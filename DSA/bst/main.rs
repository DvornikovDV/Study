// Дворников Даниил ИВТ-22


use BinTree::BinaryTree::BinaryTree;
use BinTree::BinSearchTree::BinSearchTree;

fn main() {
    // Создание экземпляра узла дерева
    let mut a = BinaryTree::new(3);

    // Добавление левого и правого узлов
    a.root.as_mut().unwrap().insert_left(2);
    a.root.as_mut().unwrap().insert_right(4);

    // Добавление левого и правого узлов для левого и правого узлов
    a.root.as_mut().unwrap().left.as_mut().unwrap().insert_left(1);
    //a.root.as_mut().unwrap().left.as_mut().unwrap().drop_left();
    a.root.as_mut().unwrap().right.as_mut().unwrap().insert_right(5);
    //a.root.as_mut().unwrap().right.as_mut().unwrap().drop_right();

    // Применение функции ко всем узлам
    a.apply(|val: &mut i32| {
        *val *= 2;
    });

    // Копирование
    let mut b = a.my_clone();

    // Вывод дерева nlr, количество
    b.print_pre();
    println!("NLR  {}", b.count());

    // Глубина, Вывод дерева lnr
    println!("LNR  {}", a.depth());
    a.print_in();

    // Вывод дерева lrn
    a.print_post();

    // Бинарное дерево поиска
    let mut a = BinSearchTree::new();

    a.insert(7).insert(5).insert(4).insert(6).insert(3).insert(9).insert(10);

    // Вывод дерева nlr
    a.print_pre();
    // Путь к узлу со значением 4, следующее наибольшее для 3
    println!("-----   {:?}\n{:?}",a.bin_search_vec(&4) , a.succ(&3));
    // Удаление узла со значением 3 (и его поддерева)
    let v = 5;
    println!("remove: {}", a.remove(v));

    // Поиск узла со значением v
    println!("{:?}", a.bin_search(&v));

    a.print_pre();

}
/*
           3
       2       4
   1               5
*/
