// Дворников Даниил ИВТ-22

use std::cmp;
use crate::{BinTreeNode, Node};


/// Бинарное дерево
pub struct BinaryTree<T> {
    pub root: Node<T>,
}

impl<T> BinaryTree<T>
where
    T: Clone + std::fmt::Debug,
{
    /// Создает и возвращает экземпляр дерева со значением value
    pub fn new(value: T) -> Self {
        BinaryTree {
            root: Some(Box::new(BinTreeNode {
                value,
                left: None,
                right: None,
            })),
        }
    }

    /// Создает и возвращает копию экземпляра бинарного дерева
    pub fn my_clone(&self) -> Self {
        match self.root {
            Some(ref root) => {
                BinaryTree {
                    root: Some(Box::new(BinTreeNode {
                        value: root.value.clone(),
                        left: match root.left {
                            Some(ref node) => { Some(Box::new(node.my_clone())) },
                            None => { None },
                        },
                        right: match root.right {
                            Some(ref node) => { Some(Box::new(node.my_clone())) },
                            None => { None },
                        },
                    })),
                }
            },
            None => BinaryTree{ root: None }
        }
    }

    /// Применение замыкания ко всем узлам дерева
    /// Big(O) - O(n)
    pub fn apply<F>(&mut self, mut fun: F)
    where
        F: FnMut(&mut T),
    {
        BinaryTree::apply_fn(&mut self.root.as_mut().unwrap(), &mut fun);
    }

    /// Рекурсивно обходит все узлы дерева и применяет к ним замыкание
    fn apply_fn<F>(node: &mut Box<BinTreeNode<T>>, fun: &mut F)
    where
        F: FnMut(&mut T),
    {
        fun(&mut node.value);

        match node.left {
            Some(ref mut left) => BinaryTree::apply_fn(left, fun),
            None => {}
        }

        match node.right {
            Some(ref mut right) => BinaryTree::apply_fn(right, fun),
            None => {}
        }
    }

    /// Возвращает глубину дерева
    /// Big(O) - O(n)
    pub fn depth(&self) -> u32 {
        BinaryTree::depth_rec(&self.root.as_ref().unwrap())
    }

    /// Рекурсивно считает глубину
    fn depth_rec(node: &BinTreeNode<T>) -> u32 {
        let left: u32 = match node.left {
            Some(ref left) => BinaryTree::depth_rec(left),
            None => 0,
        };

        let right: u32 = match node.right {
            Some(ref right) => BinaryTree::depth_rec(right),
            None => 0,
        };

        cmp::max(left, right) + 1
    }

    /// Возвращает кол-во узлов
    /// Big(O) - O(n)
    pub fn count(&self) -> u32 {
        let mut result: u32 = 0;

        BinaryTree::preorder_count(&self.root.as_ref().unwrap(), &mut result);

        result
    }

    /// Рекурсивно обходит дерево, считает узлы
    fn preorder_count(node: &BinTreeNode<T>, res: &mut u32) {
        *res += 1;

        match node.left {
            Some(ref left) => BinaryTree::preorder_count(left, res),
            None => {}
        }

        match node.right {
            Some(ref right) => BinaryTree::preorder_count(right, res),
            None => {}
        }
    }

    /// Возвращает вектор из всех узлов дерева, полученный с помощью preorder обхода
    pub fn to_vec_pre(&self) -> Vec<T> {
        let mut result = Vec::new();

        BinaryTree::preorder(&self.root.as_ref().unwrap(), &mut result);

        result
    }

    /// Печатает в консоль вектор из всех узлов дерева
    pub fn print_pre(&self) {
        println!("{:?}", self.to_vec_pre());
    }

    /// NLR
    /// Big(O) - O(n)
    fn preorder(node: &BinTreeNode<T>, res: &mut Vec<T>) {
        res.push(node.value.clone());

        match node.left {
            Some(ref left) => BinaryTree::preorder(left, res),
            None => {}
        }

        match node.right {
            Some(ref right) => BinaryTree::preorder(right, res),
            None => {}
        }
    }

    /// Возвращает вектор из всех узлов дерева, полученный с помощью inorder обхода
    pub fn to_vec_in(&self) -> Vec<T> {
        let mut result = Vec::new();

        BinaryTree::inorder(&self.root.as_ref().unwrap(), &mut result);

        result
    }

    /// Печатает в консоль вектор из всех узлов дерева
    pub fn print_in(&self) {
        println!("{:?}", self.to_vec_in());
    }

    /// LNR
    /// Big(O) - O(n)
    fn inorder(node: &BinTreeNode<T>, res: &mut Vec<T>) {
        match node.left {
            Some(ref left) => BinaryTree::inorder(left, res),
            None => {}
        }

        res.push(node.value.clone());

        match node.right {
            Some(ref right) => BinaryTree::inorder(right, res),
            None => {}
        }
    }

    /// Возвращает вектор из всех узлов дерева, полученный с помощью postorder обхода
    pub fn to_vec_post(&self) -> Vec<T> {
        let mut result = Vec::new();

        BinaryTree::postorder(&self.root.as_ref().unwrap(), &mut result);

        result
    }

    /// Печатает в консоль вектор из всех узлов дерева
    pub fn print_post(&self) {
        println!("{:?}", self.to_vec_post());
    }

    /// LRN
    /// Big(O) - O(n)
    fn postorder(node: &BinTreeNode<T>, res: &mut Vec<T>) {
        match node.left {
            Some(ref left) => BinaryTree::postorder(left, res),
            None => {}
        }

        match node.right {
            Some(ref right) => BinaryTree::postorder(right, res),
            None => {}
        }

        res.push(node.value.clone());
    }
}