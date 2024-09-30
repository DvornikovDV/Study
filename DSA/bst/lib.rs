// Дворников Даниил ИВТ-22

pub mod BinSearchTree;
pub mod BinaryTree;

/// Псевдоним типа для указателя на узел дерева
type Node<T> = Option<Box<BinTreeNode<T>>>;

/// Узел бинарного дерева
#[derive(Clone, Debug, PartialEq)]
pub struct BinTreeNode<T> {
    pub left: Node<T>,
    pub right: Node<T>,
    pub value: T,
}

impl<T> BinTreeNode<T>
where
    T: Clone {
    /// Создает и возвращает экземпляр узда дерева со значением value
    pub fn new(value: T) -> Self {
        BinTreeNode {
            value,
            left: None,
            right: None,
        }
    }

    /// Создает и возвращает копию экземпляра узда дерева
    pub fn my_clone(&self) -> Self {
        BinTreeNode{
            value: self.value.clone(),
            left: match self.left {
                Some(ref node) => { Some(Box::new(node.my_clone())) },
                None => { None },
            },
            right: match self.right {
                Some(ref node) => { Some(Box::new(node.my_clone())) },
                None => { None },
            },
        }
    }

    /// Создает для текущего узла левый узел со значением value
    pub fn insert_left(&mut self, value: T) {
        self.left = Some(Box::new(BinTreeNode {
            value,
            left: None,
            right: None,
        }));
    }

    /// Создает для текущего узла правый узел со значением value
    pub fn insert_right(&mut self, value: T) {
        self.right = Some(Box::new(BinTreeNode {
            value,
            left: None,
            right: None,
        }));
    }

    /// Удаляет левое поддерево
    pub fn drop_left(&mut self) {
        drop(self.left.take());
    }

    /// Удаляет правое поддерево
    pub fn drop_right(&mut self) {
        drop(self.right.take());
    }
}

pub fn drop<T>(_t: T) {}

