// Дворников Даниил ИВТ-22

use std::cmp::Ordering;
use crate::{BinTreeNode, Node};


/// Бинарное дерево поиска, на основе BinTreeNode, узла бинарного дерева
#[derive(Clone)]
pub struct BinSearchTree<T>{
    root: Node<T>,
}

impl<T> BinSearchTree<T>
/// Шаблонный тип T должен реализовывать следующие трейты: Ord - сравнение без неопределенных случаев, Debug - отладочный вывод в консоль, Clone - клонирование
where T: Ord + std::fmt::Debug + Clone, BinTreeNode<T>: PartialEq {
    /// Создает и возвращает экземпляр дерева без узлов
    pub fn new() -> Self {
        BinSearchTree {
            root: None
        }
    }

    /// Добавление нового узла со значением value
    /// Big(O) - O(n) в худшем случае(дерево, как связанный список),
    /// O(log n) в сбалансированном дереве(высоты поддеревьев отличаются не более, чем на единицу)
    /// Средний случай - O(log n)
    pub fn insert(&mut self, value: T) -> &mut Self {
        let mut subtree = &mut self.root;

        // Это можно прочитать так: "Пока `let` деструктурирует `subtree` в
        // `Some(i)`, выполняем блок (`{}`). В противном случае `break`.
        while let Some(ref mut node) = subtree {
            subtree = match value.cmp(&node.value) {
                Ordering::Equal => return self,
                Ordering::Less => &mut node.left,
                Ordering::Greater => &mut node.right,
            }
        }

        *subtree = Box::new(BinTreeNode { value, left: None, right: None }).into();
        self
    }

    /// Поиск value в дереве, возвращает Some(изменяемую ссылку на узел), либо None, если не найдено
    /// Big(O) - O(log n)
    pub fn bin_search(&mut self, value: &T) -> Option<&mut Box<BinTreeNode<T>>>{
        let mut res = &mut self.root;
        while let Some(ref mut node) = res {
            res = match value.cmp(&node.value) {
                Ordering::Equal => return Some(node),
                Ordering::Less => &mut node.left,
                Ordering::Greater => &mut node.right,
            }// option + continue with None
        }
        None
    }

    /// Поиск value в дереве, возвращает Some(вектор, содержащий значения узлов, находящихся на пути к искомому), либо None, если не найдено
    /// /// Big(O) - O(log n)
    pub fn bin_search_vec(&self, value: &T) -> Option<Vec<T>> {
        let mut current = &self.root;
        let mut res: Vec<T> = Vec::new();
        while let Some(ref node) = current {
            res.push(node.value.clone());

            current = match value.cmp(&node.value) {
                Ordering::Equal => return Some(res),
                Ordering::Less => { &node.left },
                Ordering::Greater => { &node.right },
            }// option + continue with None
        }
        None
    }

    /// Ищет следующее наибольшее значение для value.
    /// Возвращает Some(значение следующего наибольшего), либо None, если не найдено
    /// Big(O) - O(log n)
    pub fn succ(&mut self, value: &T) -> Option<T> {
        let first_val = match self.bin_search(value) {
            Some(node) => node,
            None => return None
        };

        match first_val.right {
            Some(ref mut node) => {
                let mut res = node;
                while let Some(ref mut node) = res.left{
                    res = node;
                };
                return Some(res.value.clone());

            },
            None => { let path = self.bin_search_vec(value).unwrap();
                for i in path.iter().rev() {
                    if *i > *value {
                        return Some(self.bin_search(i).unwrap().value.clone());
                    }
                }
                None
            }
        }
    }

    /// Удаление узла со значением value
    /// Big(O) - O(log n)
    pub fn remove(&mut self, value: T) -> bool {
        match self.root.as_mut() {
            Some(node) => {
                match value.cmp(&node.value) {
                    Ordering::Equal => {
                        drop(self.root.take());
                        true
                    },
                    _ => {
                        BinSearchTree::remove_rec(node, value)
                    },
                }
            },
            None => false
        }
    }

    /// Рекурсивный поиск и удаление узла
    fn remove_rec(node: &mut Box<BinTreeNode<T>>, value: T) -> bool {
        /// Ищет узел, левый или правый которого являются искомыми
        match value.cmp(&node.value) {
            Ordering::Less => { // Искомое меньше текущего - проверить левый
                match node.left {
                    Some(ref mut left) => {
                        match value.cmp(&left.value) {
                            Ordering::Equal => { // Удаляемое найдено

                                if left.left == None && left.right == None {
                                    // Потомков нет - заменить узел на None, удалить
                                    drop(node.left.take());
                                    true
                                }
                                else if left.left == None && left.right != None {
                                    // Есть только правый - клонировать данные, удалить правый
                                    left.value = left.right.as_ref().unwrap().value.clone();
                                    left.left = left.right.as_ref().unwrap().left.clone();
                                    left.right = left.right.as_ref().unwrap().right.clone();
                                    drop(left.right.take());
                                    true
                                }
                                else if left.left != None && left.right == None {
                                    // Есть только левый - клонировать данные, удалить левый
                                    left.value = left.left.as_ref().unwrap().value.clone();
                                    left.right = left.left.as_ref().unwrap().right.clone();
                                    left.left = left.left.as_ref().unwrap().left.clone();
                                    drop(left.left.take());
                                    true
                                }
                                else {
                                    // Оба потомка - найти следующее наибольшее, заменить только значение, удалить succ
                                    let mut res = left.right.as_mut().unwrap();
                                    while let Some(ref mut node) = res.left{
                                        res = node;
                                    }
                                    left.value = res.value.clone();
                                    let value = res.value.clone();
                                    BinSearchTree::remove_rec(left, value)
                                }

                            },
                            _ => BinSearchTree::remove_rec(left, value), // Левое не равно - проверить его потомков
                        }
                    },
                    None => false // Левого нет - удаляемое не найдено
                }
            },
            _ => { // Искомое больше или равно текущему - проверить правый( Искомое может быть равно текущему только в случае, если у удаляемого было 2 потомка - 4 блок в ветвлении )
                match node.right {
                    Some(ref mut right) => {
                        match value.cmp(&right.value) {
                            Ordering::Equal => {
                                if right.left == None && right.right == None {
                                    // Потомков нет - заменить узел на None, удалить
                                    drop(node.right.take());
                                    true
                                }
                                else if right.left == None && right.right != None {
                                    // Есть только правый - клонировать данные, удалить правый
                                    right.value = right.right.as_ref().unwrap().value.clone();
                                    right.left = right.right.as_ref().unwrap().left.clone();
                                    right.right = right.right.as_ref().unwrap().right.clone();
                                    drop(right.right.take());
                                    true
                                }
                                else if right.left != None && right.right == None {
                                    // Есть только левый - клонировать данные, удалить левый
                                    right.value = right.left.as_ref().unwrap().value.clone();
                                    right.right = right.left.as_ref().unwrap().right.clone();
                                    right.left = right.left.as_ref().unwrap().left.clone();
                                    drop(right.left.take());
                                    true
                                }
                                else {
                                    // Оба потомка - найти следующее наибольшее, заменить только значение, удалить succ
                                    let mut res = right.right.as_mut().unwrap();
                                    while let Some(ref mut node) = res.left{
                                        res = node;
                                    }
                                    right.value = res.value.clone();
                                    let value = res.value.clone();
                                    BinSearchTree::remove_rec(right, value)
                                }
                            },
                            _ => BinSearchTree::remove_rec(right, value), // Правое не равно - проверить его потомков
                        }
                    },
                    None => false // Правого нет - удаляемое не найдено
                }
            },
        }
    }

    /// Возвращает вектор из всех узлов дерева, полученный с помощью preorder обхода
    /// Big(O) - O(n)
    pub fn to_vec_pre(&self) -> Vec<T> {
        let mut result = Vec::new();

        match self.root.as_ref() {
            Some(node) => {BinSearchTree::preorder(node, &mut result)},
            None => {},
        }

        result
    }

    /// Печатает в консоль вектор из всех узлов дерева
    /// Big(O) - O(n)
    pub fn print_pre(&self) {
        println!("{:?}", self.to_vec_pre());
    }

    /// NLR
    /// Big(O) - O(n)
    fn preorder(node: &BinTreeNode<T>, res: &mut Vec<T>) {
        res.push(node.value.clone());

        match node.left {
            Some(ref left) => BinSearchTree::preorder(left, res),
            None => {}
        }

        match node.right {
            Some(ref right) => BinSearchTree::preorder(right, res),
            None => {}
        }
    }

}