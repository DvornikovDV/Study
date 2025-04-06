from lib_async import *

async def main():
    """
        Асинхронный скрапер - проверяет, есть ли в config запись о запрашиваемом url и, если есть, скрапит с него данные по селекторам
        Принимает пути к config, json с рузультатами и адрес сайта
        Записывает dataframe результаты в json по указанному пути
    """
    if len(sys.argv) != 4:
        sys.exit("Input your <config_file> <results_path> <base_url>\nExample:\npy main_async.py config.json habr_news_data.json https://habr.com/ru/flows/develop/news/")

    config_file = sys.argv[1]
    results_path = sys.argv[2]
    base_url = sys.argv[3]

    config = load_config(config_file)
    
    if base_url in config:
        news_data = await scrape_habr_news(base_url, config[base_url])
        
        if news_data:
            save_to_json(news_data, results_path)
    else:
        print(f"No configuration found for base url: {base_url}")

if __name__ == "__main__":
    asyncio.run(main())