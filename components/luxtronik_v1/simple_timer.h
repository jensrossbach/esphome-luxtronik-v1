/*
 * Copyright (c) 2024 Jens-Uwe Rossbach
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

#include <chrono>
#include <functional>


namespace esphome::luxtronik_v1
{
    class SimpleTimer
    {
    public:
        using Callback = std::function<void()>;

        SimpleTimer()
            : m_running(false)
            , m_start_time()
            , m_duration()
            , m_callback()
        {
        }

        template<class R, class P>
        void schedule(const std::chrono::duration<R, P>& duration, Callback&& cb)
        {
            m_start_time = std::chrono::steady_clock::now();
            m_duration = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            m_callback = std::move(cb);

            m_running = true;
        }

        void cancel()
        {
            m_running = false;
        }

        void loop()
        {
            if (m_running)
            {
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                                            std::chrono::steady_clock::now() - m_start_time);

                if (diff >= m_duration)
                {
                    m_running = false;

                    Callback cb = std::move(m_callback);
                    cb();  // do not call m_callback directly as it might be reassigned during call
                }
            }
        }

    private:
        bool m_running;
        std::chrono::time_point<std::chrono::steady_clock> m_start_time;
        std::chrono::milliseconds m_duration;
        Callback m_callback;
    };
}  // namespace esphome::luxtronik_v1
