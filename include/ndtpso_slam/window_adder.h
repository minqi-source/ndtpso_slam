#ifndef WINDOW_ADDER_H
#define WINDOW_ADDER_H
#include <stdexcept>
#include <vector>

template <typename TYPE>
class WindowAdder {
private:
    std::vector<TYPE> s_window;
    TYPE s_sum;
    size_t s_counter{ 0 }, s_size{ 0 };

public:
    WindowAdder(size_t size, TYPE init_val)
    {
        this->s_sum = init_val;
        this->s_size = size;
        this->s_window = std::vector<TYPE>(size);
    }

    void add(TYPE val)
    {
        this->s_sum = this->s_sum + val - this->s_window[this->s_counter];
        this->s_window[this->s_counter] = val;
    }

    void addAndIncrement(TYPE val)
    {
        this->add(val);
        this->increment();
    }

    void increment()
    {
        this->s_counter = (this->s_counter + 1) % this->s_size;
    }

    TYPE sum()
    {
        return this->s_sum;
    }

    TYPE get(size_t index)
    {
        return this->s_window[index < this->s_size ? index : 0];
    }

    TYPE get()
    {
        return this->s_window[this->s_counter];
    }

    TYPE* getPtr(size_t index)
    {
        if (index < this->s_size) {
            return &this->s_window[index];
        }

        throw std::invalid_argument("Invalid index");
    }

    TYPE* getPtr()
    {
        return &this->s_window[this->s_counter];
    }

    void set(size_t index, TYPE val)
    {
        if (index < this->s_size) {
            this->s_window[index] = val;
            return;
        }

        throw std::invalid_argument("Invalid index");
    }

    void set(TYPE val)
    {
        this->s_window[this->s_counter] = val;
    }

    size_t getCounter()
    {
        return this->s_counter;
    }
};

#endif // WINDOW_ADDER_H
