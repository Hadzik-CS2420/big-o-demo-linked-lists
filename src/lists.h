// ============================================================================
// Minimal singly and doubly linked lists of ints, for the Big O demos
// ============================================================================
// The same designs as CT 08 - CT 10, cut down to what the benchmarks call.
// Each method's comment says its Big O and why.
// ============================================================================
#pragma once

#include <cstddef>

// ── Singly linked list ──────────────────────────────────────────────────────

struct Node {
    int data;
    Node* next;
    Node(int value, Node* next = nullptr) : data{value}, next{next} {}
};

class SinglyLinkedList {
public:
    SinglyLinkedList() = default;
    ~SinglyLinkedList() {
        while (head_) {
            Node* temp = head_;
            head_ = head_->next;
            delete temp;
        }
    }
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    // O(1) -- just update the head pointer
    void push_front(int value) {
        head_ = new Node(value, head_);
        ++size_;
    }

    // O(n) -- must walk to the end every time
    void push_back(int value) {
        Node* new_node = new Node(value);
        if (!head_) {
            head_ = new_node;
        } else {
            Node* current = head_;
            while (current->next) current = current->next;
            current->next = new_node;
        }
        ++size_;
    }

    // O(1) -- just update the head pointer
    void pop_front() {
        if (!head_) return;
        Node* temp = head_;
        head_ = head_->next;
        delete temp;
        --size_;
    }

    // O(n) -- must walk to the second-to-last node (trailing pointer)
    void pop_back() {
        if (!head_) return;
        if (!head_->next) {
            delete head_;
            head_ = nullptr;
            --size_;
            return;
        }
        Node* previous = head_;
        Node* current = head_->next;
        while (current->next) {
            previous = current;
            current = current->next;
        }
        previous->next = nullptr;
        delete current;
        --size_;
    }

    // O(n) -- no address arithmetic: follow i next pointers to get there
    int get(std::size_t i) const {
        Node* current = head_;
        while (i-- > 0) current = current->next;
        return current->data;
    }

    // O(n) -- may have to look at every node
    bool contains(int value) const {
        for (Node* current = head_; current; current = current->next)
            if (current->data == value) return true;
        return false;
    }

    std::size_t size() const { return size_; }

private:
    Node* head_ = nullptr;
    std::size_t size_ = 0;
};

// ── Doubly linked list ──────────────────────────────────────────────────────

struct DoublyNode {
    int data;
    DoublyNode* next;
    DoublyNode* prev;
    DoublyNode(int value, DoublyNode* next = nullptr, DoublyNode* prev = nullptr)
        : data{value}, next{next}, prev{prev} {}
};

class DoublyLinkedList {
public:
    DoublyLinkedList() = default;
    ~DoublyLinkedList() {
        while (head_) {
            DoublyNode* temp = head_;
            head_ = head_->next;
            delete temp;
        }
    }
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    // O(1) -- direct pointer update
    void push_front(int value) {
        DoublyNode* new_node = new DoublyNode(value, head_);
        if (head_) head_->prev = new_node;
        else tail_ = new_node;
        head_ = new_node;
        ++size_;
    }

    // O(1) -- jump straight to tail_
    void push_back(int value) {
        DoublyNode* new_node = new DoublyNode(value, nullptr, tail_);
        if (tail_) tail_->next = new_node;
        else head_ = new_node;
        tail_ = new_node;
        ++size_;
    }

    // O(1) -- direct pointer update
    void pop_front() {
        if (!head_) return;
        DoublyNode* temp = head_;
        head_ = head_->next;
        if (head_) head_->prev = nullptr;
        else tail_ = nullptr;
        delete temp;
        --size_;
    }

    // O(1) -- step back once through tail_->prev
    void pop_back() {
        if (!tail_) return;
        DoublyNode* temp = tail_;
        tail_ = tail_->prev;
        if (tail_) tail_->next = nullptr;
        else head_ = nullptr;
        delete temp;
        --size_;
    }

    // O(n) -- still a walk; prev only lets you start from the nearer end
    int get(std::size_t i) const {
        if (i < size_ / 2) {
            DoublyNode* current = head_;
            while (i-- > 0) current = current->next;
            return current->data;
        }
        DoublyNode* current = tail_;
        for (std::size_t k = size_ - 1; k > i; --k) current = current->prev;
        return current->data;
    }

    // O(n) -- may have to look at every node
    bool contains(int value) const {
        for (DoublyNode* current = head_; current; current = current->next)
            if (current->data == value) return true;
        return false;
    }

    std::size_t size() const { return size_; }

private:
    DoublyNode* head_ = nullptr;
    DoublyNode* tail_ = nullptr;
    std::size_t size_ = 0;
};
