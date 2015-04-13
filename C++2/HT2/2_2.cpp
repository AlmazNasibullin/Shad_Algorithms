#include <ostream>
#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <ctime>
#include <chrono>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <iomanip>

using namespace std::chrono;

/*
template <typename Type>
class TBlockingQueue {
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<Type> queue_;

public:
    void push(Type const& value) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push_front(value);
        }
        cv_.notify_one();
    }

    Type pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] {
            return !this->queue_.empty();
        });
        Type extracted(queue_.front());
        queue_.pop();
        return extracted;
    }

    bool tryPop(Type &value, milliseconds duration) {
        std::unique_lock<std::mutex> lock(this->mutex_);
        if (!cv_.wait_for(lock, duration, [this] {
                    return !this->queue_.empty();
                })) {
            return false;
        }
        value = queue_.fornt();
        queue_.pop();
        return true;
    }

    size_t size() const {
        return queue_.size();
    }
};

template <typename Type>
void execute(TBlockingQueue<TTask> &queue,
            const matrix::TMatrix<Type> &firstMatrix,
            const matrix::TMatrix<Type> &secondMatrix,
            matrix::TMatrix<Type> &resultMatrix) {
    for (;;) {
        TTask task = queue.pop();
        if (task.final_) {
            return;
        }
        matrix::multiplyMatrix(firstMatrix, secondMatrix, resultMatrix,
            task.leftUp_, task.rightDown_);
    }
}
*/

namespace matrix {

    template <typename Type>
    class TMatrix {
    private:
        int size_;
        std::vector<std::vector<Type> > matrix_;

    public:
        explicit TMatrix(int size) : size_(size) {
            matrix_.resize(size_);
            for (std::vector<Type> &row : matrix_) {
                row.resize(size_);
            }
        }

        TMatrix(const TMatrix<Type> &other) {
            matrix_.clear();
            size_ = other.size_;
            matrix_.resize(size_);
            for (int i = 0; i < size_; ++i) {
                matrix_[i] = other.matrix_[i];
            }
        }

        TMatrix<Type>& operator= (const TMatrix<Type> &other) {
            matrix_.clear();
            size_ = other.size_;
            matrix_.resize(size_);
            for (int i = 0; i < size_; ++i) {
                matrix_[i] = other.matrix_[i];
            }
            return *this;
        }

        int size() const {
            return size_;
        }

        Type& operator() (int row, int column) {
            return matrix_[row][column];
        }

        Type operator() (int row, int column) const {
            return matrix_[row][column];
        }

        bool operator== (const TMatrix<Type> &other) {
            if (size_ != other.size_) {
                return false;
            }
            for (int i = 0; i < size_; ++i) {
                for (int j = 0; j < size_; ++j) {
                    if (matrix_[i][j] != other.matrix_[i][j]) {
                        return false;
                    }
                }
            }
            return true;
        }

        bool operator!= (const TMatrix<Type> &other) {
            return !(*this == other);
        }

        friend std::ostream &operator<< (std::ostream &output,
                                        const TMatrix<Type> &matrix) {
            for (int i = 0; i < matrix.size_; ++i) {
                for (int j = 0; j < matrix.size_; ++j) {
                    output << matrix(i, j) << " ";
                }
                output << "\n";
            }
            return output;
        }
    };

    template <typename Type>
    void fillIntegerMatrixRandomly(TMatrix<Type> &matrix,
                            Type leftBorder,
                            Type rightBorder) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(leftBorder, rightBorder);
        for (int i = 0; i < matrix.size(); ++i) {
            for (int j = 0; j < matrix.size(); ++j) {
                matrix(i, j) = dis(gen);
            }
        }
    }

    struct CellNumber {
        int row;
        int column;

        CellNumber(int row_ = 0, int column_ = 0)
            : row(row_),
            column(column_) {}
    };

    template <typename Type>
    void multiplyVectors(const TMatrix<Type> &firstMatrix,
                        const TMatrix<Type> &secondMatrix,
                        TMatrix<Type> &resultMatrix,
                        int row,
                        int column,
                        const CellNumber &leftUp) {
        resultMatrix(row, column) = Type();
        int size = resultMatrix.size();
        for (int index = 0; index < size; ++index) {
            resultMatrix(row, column)
                += firstMatrix(row, index) * secondMatrix(index, column);
        }
    }

    template <typename Type>
    void multiplyMatrix(const TMatrix<Type> &firstMatrix,
                        const TMatrix<Type> &secondMatrix,
                        TMatrix<Type> &resultMatrix,
                        const CellNumber &leftUp,
                        const CellNumber &rightDown) {
        for (int row = leftUp.row; row <= rightDown.row; ++row) {
            for (int column = leftUp.column; column <= rightDown.column;
                    ++column) {
                multiplyVectors(firstMatrix, secondMatrix, resultMatrix,
                    row, column, leftUp);
            }
        }
    }

}; // namespace matrix

struct TTask {
    matrix::CellNumber leftUp_;
    matrix::CellNumber rightDown_;
    bool final_;

    TTask(const matrix::CellNumber &leftUp,
            const matrix::CellNumber &rightDown,
            bool final)
        : leftUp_(leftUp),
        rightDown_(rightDown),
        final_(final) {}
};

void runComparison() {
    const int maxThreadCount = 10;
    const std::vector<int> sizes = {128, 256, 512, 1024};
    std::cout << "Размер матрицы x Кол-во потоков\n";
    for (int size : sizes) {
        matrix::TMatrix<int> first(size);
        matrix::TMatrix<int> second(size);
        matrix::TMatrix<int> resultForOneThread(size);
        matrix::TMatrix<int> result(size);
        matrix::fillIntegerMatrixRandomly(first, 0, 5);
        matrix::fillIntegerMatrixRandomly(second, 0, 5);
        
        for (int threadsCount = 1; threadsCount < maxThreadCount;
                ++threadsCount) {
            // В случае перемножения нескольких пар матриц можно было бы
            // использовать блокирующую очередь, складывая в нее таски.
            std::vector<int> forEach;
            forEach.resize(threadsCount, size / threadsCount);
            int rest = size - threadsCount * (size / threadsCount);
            for (int i = 0; i < rest; ++i) {
                ++forEach[i];
            }

            std::vector<std::thread> threads;
            high_resolution_clock::time_point firstTP = high_resolution_clock::now();
            int curStart = 0;
            for (int i = 0; i < threadsCount; ++i) {
                if (threadsCount == 1) {
                    threads.push_back(std::thread(matrix::multiplyMatrix<int>,
                        std::ref(first), std::ref(second),
                        std::ref(resultForOneThread), matrix::CellNumber(curStart, 0),
                        matrix::CellNumber(curStart + forEach[i] - 1, size - 1)));
                } else {
                    threads.push_back(std::thread(matrix::multiplyMatrix<int>,
                        std::ref(first), std::ref(second), std::ref(result),
                        matrix::CellNumber(curStart, 0),
                        matrix::CellNumber(curStart + forEach[i] - 1, size - 1)));
                }
                curStart += forEach[i];
            }
            for (int i = 0; i < threadsCount; ++i) {
                threads[i].join();
            }
            high_resolution_clock::time_point secondTP
                = high_resolution_clock::now();
            duration<double> timeSpan
                = duration_cast<duration<double>>(secondTP - firstTP);
            std::cout << std::setprecision(2) << timeSpan.count() << "\t"
                << std::flush;

            if (threadsCount != 1 && result != resultForOneThread) {
                std::cout << "\nIncorrect answer " << threadsCount << "threads\n";
                return;
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    runComparison();
    return 0;
}
