/**
 * @brief Task 2
 * 
 На языке С++, написать минимум по 2 класса реализовывающих циклический буфер.

 Объяснить плюсы и минусы каждой реализации.
 * 
 */


// По хорошему, я бы добавил возможность использования нестандартных аллокаторов памяти
// По опыту могу сказать, что это хорошо сказывается на производительности, особенно в циклических буферах.


/**
 * @brief первый класс, не является потокобезопасным
 */
template <typename T, int N>
class CircularBuffer1 final {
public:
    using type = T;
    using reference_type = std::add_lvalue_reference_t<T>;

private:
    T   _buffer[N];
    int _head;
    int _tail;
    int _count;
    const int _cap = N;

public:
    CircularBuffer1() { clear(); }

    CircularBuffer1(CircularBuffer1&& inst) { 
        *this = std::move(inst);
    }
    CircularBuffer1(const CircularBuffer1& inst) {
        *this = inst;
    }
    CircularBuffer1& operator=(CircularBuffer1&& inst) {
        if (inst._count > _cap)
            throw std::overflow_error("...");

        clear();

        for (std::size_t i = 0; i < inst._count; ++i) {
            push_back(std::move(inst.back))
        }
    }
    CircularBuffer1& operator=(const CircularBuffer1& inst) {
        if (inst._count > _cap)
            throw std::overflow_error("...");

        clear();

        for (std::size_t i = 0; i < inst._count; ++i) {
            push_back(inst.back)
        }
    }

    ~CircularBuffer1() = default;

    bool isFull() const {
        return _count == N;
    }

    bool isEmpty() const {
        return _count == 0;
    }

    std::size_t size() const {
        return _count;
    }

    void clear() {
        _head = 0;
        _tail = 0;
        _count = 0;
    }

    reference_type front() {
        return _buffer[_head];
    }

    const reference_type front() const {
        return _buffer[_head];
    }

    reference_type back() {
        return _buffer[_tail - 1];
    }

    const reference_type back() const {
        return _buffer[_tail - 1];
    }

    template<typename U>
    void push_back(U&& value) {
        if (is_full()) {
            // Handle error or overflow condition
            throw std::overflow_error("...");
            return;
        }

        _buffer[_tail] = std::forward<U>(value);
        _tail = (_tail + 1) % N;
        ++_count;
    }

    void pop_front() {
        if (is_empty()) {
            // ... mb throw?
            return;
        }
        _head = (_head + 1) % N;
        --_count;
    }
};



/**
 * @brief второй класс, для использования в контексте нескольких потоков
 */
template <typename T, int N>
class CircularBuffer2 final {
public:
    using type = T;
    using reference_type = std::add_lvalue_reference_t<T>;

private:
    T   _buffer[N];
    int _head;
    int _tail;
    int _count;
    const int _cap = N;
    std::mutex _lock;

public:
    CircularBuffer2() { clear(); }

    CircularBuffer2(CircularBuffer2&& inst) { 
        if (inst._count > _cap)
            throw std::overflow_error("...");

        clear();

        for (std::size_t i = 0; i < inst._count; ++i) {
            push_back(std::move(inst.back))
        }
    }
    CircularBuffer2(const CircularBuffer2& inst) {
        std::lock_guard<std::mutex> lock(_lock);
        
        if (inst._count > _cap)
            throw std::overflow_error("...");

        clear();

        for (std::size_t i = 0; i < inst._count; ++i) {
            push_back(inst.back)
        }
    }

    CircularBuffer2& operator=(CircularBuffer2&& inst) 
    {
        std::lock_guard<std::mutex> lock(_lock);

        if (inst._count > _cap)
            throw std::overflow_error("...");

        clear();

        for (std::size_t i = 0; i < inst._count; ++i) {
            push_back(std::move(inst.back))
        }
    }
    CircularBuffer2& operator=(const CircularBuffer2& inst) 
    {
        std::lock_guard<std::mutex> lock(_lock);

        if (inst._count > _cap)
            throw std::overflow_error("...");

        clear();

        for (std::size_t i = 0; i < inst._count; ++i) {
            push_back(inst.back)
        }
    }

    ~CircularBuffer2() = default;

    bool isFull() const {
        return _count == N;
    }

    bool isEmpty() const {
        return _count == 0;
    }

    std::size_t size() const {
        return _count;
    }

    void clear() {
        _head = 0;
        _tail = 0;
        _count = 0;
    }

    reference_type front() {
        return _buffer[_head];
    }

    const reference_type front() const {
        return _buffer[_head];
    }

    reference_type back() {
        return _buffer[_tail - 1];
    }

    const reference_type back() const {
        return _buffer[_tail - 1];
    }

    template<typename U>
    void push_back(U&& value) {
        std::lock_guard<std::mutex> lock(_lock);

        if (is_full()) {
            // Handle error or overflow condition
            throw std::overflow_error("...");
            return;
        }
        
        _buffer[_tail] = std::forward<U>(value);
        _tail = (_tail + 1) % N;
        ++_count;
    }

    void pop_front() {
        std::lock_guard<std::mutex> lock(_lock);

        if (is_empty()) {
            // ... mb throw?
            return;
        }
        _head = (_head + 1) % N;
        --_count;
    }
};