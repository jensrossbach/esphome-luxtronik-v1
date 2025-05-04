/*
 * Copyright (c) 2025 Jens-Uwe Rossbach
 *
 * This code is licensed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once


namespace esphome::luxtronik_v1
{
    template<class T>
    class SimpleQueue
    {
    public:
        SimpleQueue()
            : m_head(nullptr)
            , m_tail(nullptr)
            , m_size(0)
        {
        }

        ~SimpleQueue()
        {
            clear();
        }

        SimpleQueue(const SimpleQueue&) = delete;
        SimpleQueue(SimpleQueue&&) = delete;

        SimpleQueue& operator=(const SimpleQueue& other) = delete;
        SimpleQueue& operator=(SimpleQueue&& other) = delete;

        const T& front() const
        {
            return m_head->m_data;
        }

        T& front()
        {
            return m_head->m_data;
        }

        const T& back() const
        {
            return m_tail->m_data;
        }

        T& back()
        {
            return m_tail->m_data;
        }

        size_t size() const
        {
            return m_size;
        }

        bool empty() const
        {
            return m_size == 0;
        }

        void push_back(T&& data)
        {
            if (m_tail == nullptr)
            {
                m_tail = new ListNode(std::move(data));
                m_head = m_tail;
            }
            else
            {
                m_tail->m_next = new ListNode(std::move(data));
                m_tail = m_tail->m_next;
            }

            ++m_size;
        }

        void push_front(T&& data)
        {
            if (m_head == nullptr)
            {
                m_head = new ListNode(std::move(data));
                m_tail = m_head;
            }
            else
            {
                ListNode* next = m_head;
                m_head = new ListNode(std::move(data));
                m_head->m_next = next;
            }

            ++m_size;
        }

        void pop_front()
        {
            if (m_head != nullptr)
            {
                ListNode* next = m_head->m_next;
                delete m_head;
                m_head = next;

                if (m_head == nullptr)  // last element removed
                {
                    m_tail = nullptr;
                }

                --m_size;
            }
        }

        void clear()
        {
            while (m_head != nullptr)
            {
                ListNode* next = m_head->m_next;
                delete m_head;
                m_head = next;
            }

            m_tail = nullptr;
            m_size = 0;
        }

    private:
        struct ListNode
        {
            ListNode(T&& data)
                : m_data(std::move(data))
                , m_next(nullptr)
            {
            }

            T m_data;
            ListNode* m_next;
        };

        ListNode* m_head;
        ListNode* m_tail;
        size_t m_size;
    };
}  // namespace esphome::luxtronik_v1
