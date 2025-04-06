import aiohttp
import asyncio
from bs4 import BeautifulSoup
import pandas as pd
import json
import sys

def load_config(config_file):
    """
        Загрузка конфигурации из config_file
    """
    with open(config_file, 'r', encoding='utf-8') as f:
        return json.load(f)


# Ограничение количества одновременных запросов
MAX_CONCURRENT_REQUESTS = 10  # Максимум одновременных запросов
semaphore = asyncio.Semaphore(MAX_CONCURRENT_REQUESTS)

async def fetch_page(session, url, headers=None):
    """
        Асинхронная загрузка HTML-контента
        Принимает сессию session, адрес страницы url, заголовки отправителей запросов headers (для обхода ограничений по запросам)
        Возвращает либо html код страницы, либо None
    """
    async with semaphore:  # Ограничиваем количество одновременных запросов
        try:
            async with session.get(url, headers=headers, timeout=10) as response:
                response.raise_for_status()
                return await response.text()
        except Exception as e:
            print(f"Error fetching {url}: {e}")
            return None
            

def parse_news_data(html_content, config):
    """
        Парсинг данных новостей
        Принимает код страницы html_content, конфиг с селекторами config
        Возвращает 
    """
    soup = BeautifulSoup(html_content, 'html.parser')
    articles = soup.find_all('article', class_='tm-articles-list__item')
    data = []

    for article in articles:
        # Заголовок
        title_tag = article.select_one(config['title_selector'])
        title = title_tag.text.strip() if title_tag else None

        # Дата
        date_tag = article.select_one(config['date_selector'])
        date = date_tag['datetime'] if date_tag and 'datetime' in date_tag.attrs else None

        # Автор
        author_tag = article.select_one(config['author_selector'])
        author = author_tag.text.strip() if author_tag else None

        # Хабы
        hubs_tags = article.select(config['hubs_selector'])
        hubs = [hub.text.strip() for hub in hubs_tags] if hubs_tags else []

        # Сохраняем данные
        data.append({
            'title': title,
            'date': date,
            'author': author,
            'hubs': hubs
        })

    return data


async def scrape_habr_news(base_url, config):
    """
        Обработка пагинации с использованием asyncio. Запускает задачи по получению кода страниц и извлечения из них данных.
        Принимает адрес сайта base_url, параметры для скрапинга config
        Возвращает список словарей с извлеченными данными
    """
    all_data = []
    current_page = 1
    tasks = []

    async with aiohttp.ClientSession() as session:
        while True:
            url = f"{base_url}page{current_page}/"
            print(f"Scraping page: {url}")
            
            # Создаем задачу для каждой страницы
            task = asyncio.create_task(fetch_and_parse(session, url, config))
            tasks.append(task)
            
            # Поиск ссылки на следующую страницу
            html_content = await fetch_page(session, url, headers=config.get('headers'))
            if not html_content:
                break
            
            soup = BeautifulSoup(html_content, 'html.parser')
            next_page = soup.select_one(config['pagination_selector'])
            if not next_page or 'href' not in next_page.attrs:
                break
            
            current_page += 1

        # Ждем завершения всех задач
        results = await asyncio.gather(*tasks)
        for result in results:
            all_data.extend(result)

    return all_data


async def fetch_and_parse(session, url, config):
    """
        Выполненяет запрос кода страницы и парсинг данных
        Принимает сессию session, адрес страницы url, параметры для текущего url config
        Возвращает список словарей с данными, либо пустой список, если данных не было
    """
    html_content = await fetch_page(session, url, headers=config.get('headers'))
    if html_content:
        return parse_news_data(html_content, config)
    return []


def save_to_json(data, filename):
    """
        Сохранение данных data в JSON по пути filename
    """
    df = pd.DataFrame(data)
    df.to_json(filename, orient='records', lines=True, force_ascii=False)
    print(f"Data saved to {filename}")