use std::collections::LinkedList;

/// Обертка для чужого списка
pub struct List<T> (pub LinkedList<T>);

/// Преобразование в итератор для списка
impl<T> IntoIterator for List<T> {
    type Item = T;
    type IntoIter = IntoIter<T>;

    /// Преобразует список в итератор, владеющий элементами
    fn into_iter(self) -> IntoIter<T> {
        IntoIter { list: self.0 }
    }
}

/// Итератор, поглощает список
pub struct IntoIter<T> {
    list: LinkedList<T>,
}

impl<T> Iterator for IntoIter<T> {
    type Item = T;

    fn next(&mut self) -> Option<T> {
        self.list.pop_front()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn into_iter() {
        let mut list = List(LinkedList::new());
        let mut count = 0;

        list.0.push_back(1);
        list.0.push_back(2);
        list.0.push_back(3);
        list.0.push_back(4);
        list.0.push_back(5);

        for i in list {
            count += 1;
            assert_eq!(i, count);
        }

    }
}

/*
use std::collections::LinkedList;
use std::marker::PhantomData;
use std::ptr::NonNull;

struct Node<T> {
    next: Option<NonNull<Node<T>>>,
    prev: Option<NonNull<Node<T>>>,
    element: T,
}

/// Обертка
pub struct List<T> (pub LinkedList<T>);

impl<T> List<T> {
    pub fn iter(&self) -> Iter<'_, T> {
        Iter { head: self.0.head, tail: self.tail, len: self.len, marker: PhantomData }
    }
}

impl<T> IntoIterator for List<T> {
    type Item = T;
    type IntoIter = IntoIter<T>;

    /// Consumes the list into an iterator yielding elements by value.
    fn into_iter(self) -> IntoIter<T> {
        IntoIter { list: self.0 }
    }
}

pub struct IntoIter<T> {
    list: LinkedList<T>,
}

impl<T> Iterator for IntoIter<T> {
    type Item = T;

    fn next(&mut self) -> Option<T> {
        self.list.pop_front()
    }
}


///
impl<'a, T> IntoIterator for &'a List<T> {
    type Item = &'a T;
    type IntoIter = Iter<'a, T>;

    fn into_iter(self) -> Iter<'a, T> {
        self.iter()
    }
}

pub struct Iter<'a, T: 'a> {
    head: Option<NonNull<Node<T>>>,
    tail: Option<NonNull<Node<T>>>,
    len: usize,
    marker: PhantomData<&'a Node<T>>,
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    #[inline]
    fn next(&mut self) -> Option<&'a T> {
        if self.len == 0 {
            None
        } else {
            self.head.map(|node| unsafe {
                // Need an unbound lifetime to get 'a
                let node = &*node.as_ptr();
                self.len -= 1;
                self.head = node.next;
                &node.element
            })
        }
    }
}


impl<'a, T, A: Allocator> IntoIterator for &'a mut LinkedList<T, A> {
    type Item = &'a mut T;
    type IntoIter = IterMut<'a, T>;

    fn into_iter(self) -> IterMut<'a, T> {
        self.iter_mut()
    }
}

pub struct IterMut<'a, T: 'a> {
    head: Option<NonNull<Node<T>>>,
    tail: Option<NonNull<Node<T>>>,
    len: usize,
    marker: PhantomData<&'a mut Node<T>>,
}

impl<'a, T> Iterator for IterMut<'a, T> {
    type Item = &'a mut T;

    #[inline]
    fn next(&mut self) -> Option<&'a mut T> {
        if self.len == 0 {
            None
        } else {
            self.head.map(|node| unsafe {
                // Need an unbound lifetime to get 'a
                let node = &mut *node.as_ptr();
                self.len -= 1;
                self.head = node.next;
                &mut node.element
            })
        }
    }
}
*/