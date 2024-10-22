// Дворников Даниил ИВТ-22

use std::cmp::Ordering;
use crate::{BinTreeNode, Node};


/// Бинарное дерево поиска, на основе BinTreeNode, узла бинарного дерева
#[derive(Clone)]
pub struct BinSearchTree<T>{
    root: Node<T>,
}

/// Конвертирует бинарное дерево поиска в итератор по неизменяемым ссылкам на элементы
impl<'a, T> IntoIterator for &'a BinSearchTree<T> {
    // Ассоциированный тип для элементов итератора
    type Item = &'a Node<T>;
    // Ассоциированный тип для итератора
    type IntoIter = Iter<'a, T>;

    /// Возвращает итератор по элементам дерева
    fn into_iter(self) -> Self::IntoIter {
        // В стек собираются корень и все элементы с переходом к левому
        let mut stack = Vec::new();
        match &self.root {
            Some(_) => {
                stack.push(&self.root);

                let mut temp = &self.root.as_ref().unwrap().left;

                match temp {
                    Some(_) => {
                        stack.push(temp);
                        while let Some(_) = temp.as_ref().unwrap().left {
                            stack.push(&temp.as_ref().unwrap().left);
                            temp = &temp.as_ref().unwrap().left;
                        }
                    },
                    None => {},
                };
            },
            None => {},
        }

        // Стек передается в итератор
        Iter {
            stack
        }
    }
}

/// Итератор для бинарного дерева поиска ( & )
pub struct Iter<'a, T> {
    stack: Vec<&'a Node<T>>,
}

impl<'a, T> Iterator for Iter<'a, T> {
    // Ассоциированный тип для элементов итератора
    type Item = &'a Node<T>;

    // Возвращает следующий узел в дереве, либо None, если узлов не осталось
    fn next(&mut self) -> Option<Self::Item> {
        let result = self.stack.pop();

        match result {
            Some(result) => {
                match result.as_ref().unwrap().right {
                    Some(ref node) => {
                        let mut cur = node;
                        self.stack.push(&result.as_ref().unwrap().right);

                        while let Some(ref node) = cur.left{
                            self.stack.push(&cur.left);
                            cur = node;
                        }
                    },
                    None => {},
                };

                Some(result)
            },
            None => None,
        }
    }

}

/// Конвертирует бинарное дерево поиска в итератор по изменяемым ссылкам на элементы
impl<'a, T: Clone + Ord + std::fmt::Debug> IntoIterator for &'a mut BinSearchTree<T> {
    // Ассоциированный тип для элементов итератора
    type Item = &'a mut T;
    // Ассоциированный тип для итератора
    type IntoIter = IterMut<'a, T>;

    /// Возвращает итератор по элементам дерева
    fn into_iter(self) -> Self::IntoIter {
        IterMut {
            nodes: self.to_vec_mut()
        }
    }
}

/// Итератор для бинарного дерева поиска ( &mut )
pub struct IterMut<'a, T> {
    nodes: Vec<&'a mut T>,
}

impl<'a, T> Iterator for IterMut<'a, T> {
    type Item = &'a mut T;

    fn next(&mut self) -> Option<Self::Item> {
        self.nodes.pop()
    }
}


/// Конвертирует бинарное дерево поиска в итератор по изменяемым ссылкам на элементы
impl<T: Clone + Ord + std::fmt::Debug> IntoIterator for BinSearchTree<T> {
    // Ассоциированный тип для элементов итератора
    type Item = T;
    // Ассоциированный тип для итератора
    type IntoIter = IntoIter<T>;

    /// Возвращает итератор по элементам дерева
    fn into_iter(self) -> Self::IntoIter {
        IntoIter {
            nodes: self.to_vec_into()
        }
    }
}

/// Итератор для бинарного дерева поиска ( move )
pub struct IntoIter<T> {
    nodes: Vec<T>,
}

impl<T> Iterator for IntoIter<T> {
    type Item = T;

    fn next(&mut self) -> Option<Self::Item> {
        self.nodes.pop()
    }
}

/// Шаблонный тип T должен реализовывать следующие трейты: Ord - сравнение без неопределенных случаев, Debug - отладочный вывод в консоль, Clone - клонирование
impl<T> BinSearchTree<T>
where T: Ord + std::fmt::Debug + Clone {
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
    /// Big(O) средний - O(log n)
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
    /// /// Big(O) средний - O(log n)
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
    /// Big(O) средний - O(log n)
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
    /// Big(O) средний  - O(log n)
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
        // Ищет узел, левый или правый которого являются искомыми
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

    /// Возвращает вектор всех узлов дерева, полученный с помощью preorder обхода
    /// Big(O) средний - O(n)
    pub fn to_vec_pre(&self) -> Vec<&T> {
        let mut result = Vec::new();

        match self.root {
            Some(ref node) => {BinSearchTree::preorder(node, &mut result)},
            None => {},
        }

        result
    }

    /// Печатает в консоль вектор из всех узлов дерева
    /// Big(O) - O(n)
    pub fn print_pre(&mut self) {
        println!("{:?}", self.to_vec_pre());
    }

    /// NLR immut borrow elems
    /// Big(O) - O(n)
    fn preorder<'a>(node: &'a BinTreeNode<T>, res: &mut Vec<&'a T>) {
        res.push(&node.value);

        match node.left {
            Some(ref left) => BinSearchTree::preorder(left, res),
            None => {}
        }

        match node.right {
            Some(ref right) => BinSearchTree::preorder(right, res),
            None => {}
        }
    }

    ////////////////////////////////////////////private////////////////////////////////////////////

    /// Возвращает вектор с перемещенными в него узлами дерева
    /// Big(O) - O(n)
    fn to_vec_into(self) -> Vec<T> {
        let mut result: Vec<T> = Vec::new();

        match self.root {
            Some(node) => {BinSearchTree::preorder_into(node, &mut result)},
            None => {},
        }

        result
    }

    /// Возвращает вектор с изменяемыми ссылками на содержимое всех узлов
    /// Big(O) - O(n)
    fn to_vec_mut(&mut self) -> Vec<&mut T> {
        let mut result = Vec::new();

        match self.root.as_mut() {
            Some(node) => {BinSearchTree::preorder_mut(node, &mut result)},
            None => {},
        }

        result
    }

    /// NLR move elems
    /// Big(O) - O(n)
    fn preorder_into<'a>(node: Box<BinTreeNode<T>>, res: &mut Vec<T>) {
        res.push(node.value); //

        match node.left {
            Some(left) => BinSearchTree::preorder_into(left, res),
            None => {}
        }

        match node.right {
            Some(right) => BinSearchTree::preorder_into(right, res),
            None => {}
        }
    }

    /// NLR mut borrow elems
    /// Big(O) - O(n)
    fn preorder_mut<'a>(node: &'a mut BinTreeNode<T>, res: &mut Vec<&'a mut T>) {
        res.push(&mut node.value);

        match node.left {
            Some(ref mut left) => BinSearchTree::preorder_mut(left, res),
            None => {}
        }

        match node.right {
            Some(ref mut right) => BinSearchTree::preorder_mut(right, res),
            None => {}
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn new() {
        let bst: BinSearchTree<i32> = BinSearchTree::new();

        assert_eq!(None, bst.root)
    }

    #[test]
    fn insert() {
        let mut bst = BinSearchTree::new();

        bst.insert(4);
        let expect = 4;
        let actual = bst.root.as_ref().unwrap().value;
        assert_eq!(expect, actual);

        bst.insert(2);
        let expect = 2;
        let actual = bst.root.as_ref().unwrap().left.as_ref().unwrap().value;
        assert_eq!(expect, actual);

        bst.insert(5);
        let expect = 5;
        let actual = bst.root.as_ref().unwrap().right.as_ref().unwrap().value;
        assert_eq!(expect, actual);
    }

    #[test]
    fn to_vec_pre() {
        let mut bst = BinSearchTree::new();
        bst.insert(4);
        bst.insert(3);
        bst.insert(7);
        bst.insert(6);
        bst.insert(9);

        let expect = vec![&4, &3, &7, &6, &9];
        let actual = bst.to_vec_pre();

        assert_eq!(expect, actual);

        let bst: BinSearchTree<i32> = BinSearchTree::new();
        let expect: Vec<&i32> = Vec::new();
        let actual = bst.to_vec_pre();
    }

    #[test]
    fn insert_row() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);

        let expect = vec![&4, &3, &7, &6, &9];
        let actual = bst.to_vec_pre();

        assert_eq!(expect, actual)
    }

    #[test]
    fn bin_search() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);

        let expect = 6;
        let actual = bst.bin_search(&6).unwrap().value;
        assert_eq!(expect, actual);

        let expect = 4;
        let actual = bst.bin_search(&4).unwrap().value;
        assert_eq!(expect, actual);

        let expect = 9;
        let actual = bst.bin_search(&9).unwrap().value;
        assert_eq!(expect, actual);

        let expect = None;
        let actual = bst.bin_search(&1);
        assert_eq!(expect, actual);

        let mut bst: BinSearchTree<i32> = BinSearchTree::new();
        let expect = None;
        let actual = bst.bin_search(&1);
        assert_eq!(expect, actual);
    }

    #[test]
    fn bin_search_vec() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);

        let expect = vec![4, 7, 9];
        let actual = bst.bin_search_vec(&9).unwrap();
        assert_eq!(expect, actual);

        let expect = None;
        let actual = bst.bin_search_vec(&1);
        assert_eq!(expect, actual);

        let expect = vec![4];
        let actual = bst.bin_search_vec(&4).unwrap();
        assert_eq!(expect, actual);

        let bst: BinSearchTree<i32> = BinSearchTree::new();
        let expect = None;
        let actual = bst.bin_search_vec(&1);
        assert_eq!(expect, actual);
    }

    #[test]
    fn succ() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);

        let expect = 4;
        let actual = bst.succ(&3).unwrap();
        assert_eq!(expect, actual);

        let expect = 6;
        let actual = bst.succ(&4).unwrap();
        assert_eq!(expect, actual);

        let expect = 7;
        let actual = bst.succ(&6).unwrap();
        assert_eq!(expect, actual);

        let expect = 9;
        let actual = bst.succ(&7).unwrap();
        assert_eq!(expect, actual);

        let expect = None;
        let actual = bst.succ(&9);
        assert_eq!(expect, actual);

        let mut bst: BinSearchTree<i32> = BinSearchTree::new();
        let expect = None;
        let actual = bst.succ(&9);
        assert_eq!(expect, actual);
    }

    #[test]
    fn remove() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);

        bst.remove(9);
        let expect = vec![&4, &3, &7, &6];
        let actual = bst.to_vec_pre();
        assert_eq!(expect, actual);

        bst.insert(9);
        bst.remove(7);
        let expect = vec![&4, &3, &9, &6];
        let actual = bst.to_vec_pre();
        assert_eq!(expect, actual);

        bst.remove(6);
        let expect = vec![&4, &3, &9];
        let actual = bst.to_vec_pre();
        assert_eq!(expect, actual);

        bst.remove(3);
        let expect = vec![&4, &9];
        let actual = bst.to_vec_pre();
        assert_eq!(expect, actual);

        bst.remove(4);
        let expect: Vec<&i32>= Vec::new();
        let actual = bst.to_vec_pre();
        assert_eq!(expect, actual);

        let expect = false;
        let actual = bst.remove(4);
        assert_eq!(expect, actual);
    }

    #[test]
    fn iter_mut() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);
        let mut bit = (&mut bst).into_iter();

        let expect = 9;
        let actual = *bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 6;
        let actual = *bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 7;
        let actual = *bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 3;
        let actual = *bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 4;
        let actual = *bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);

        let mut bst: BinSearchTree<i32> = BinSearchTree::new();
        let mut bit = (&mut bst).into_iter();
        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);
    }

    #[test]
    fn into_iter() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);
        let mut bit = bst.into_iter();

        let expect = 9;
        let actual = bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 6;
        let actual = bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 7;
        let actual = bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 3;
        let actual = bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = 4;
        let actual = bit.next().unwrap();
        assert_eq!(expect, actual);

        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);

        let b: BinSearchTree<i32> = BinSearchTree::new();
        b.into_iter();
        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);

        let bst: BinSearchTree<i32> = BinSearchTree::new();
        let mut bit = bst.into_iter();
        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);
    }

    #[test]
    fn iter() {
        let mut bst = BinSearchTree::new();
        bst.insert(4).insert(3).insert(7).insert(6).insert(9);
        let mut bit = (&bst).into_iter();

        let expect = 3;
        let actual = bit.next().unwrap().as_ref().unwrap().value;
        assert_eq!(expect, actual);

        let expect = 4;
        let actual = bit.next().unwrap().as_ref().unwrap().value;
        assert_eq!(expect, actual);

        let expect = 6;
        let actual = bit.next().unwrap().as_ref().unwrap().value;
        assert_eq!(expect, actual);

        let expect = 7;
        let actual = bit.next().unwrap().as_ref().unwrap().value;
        assert_eq!(expect, actual);

        let expect = 9;
        let actual = bit.next().unwrap().as_ref().unwrap().value;
        assert_eq!(expect, actual);

        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);

        let bst: BinSearchTree<i32> = BinSearchTree::new();
        let mut bit = (&bst).into_iter();
        let expect = None;
        let actual = bit.next();
        assert_eq!(expect, actual);
    }
}