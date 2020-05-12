#pragma once

#include <anton/functors.hpp>
#include <anton/tags.hpp>
#include <anton/types.hpp>
#include <anton/utility.hpp>

namespace anton {
    template<typename T>
    class Shared_Ptr {
    private:
        struct Erased_Deleter_Base {
            virtual ~Erased_Deleter_Base() {}
            virtual void destroy(T* pointer) = 0;
        };

        template<typename FN>
        struct Erased_Deleter: Erased_Deleter_Base {
            template<typename FN_Param>
            Erased_Deleter(FN_Param&& fn): _fn(forward<FN_Param>(fn)) {}

            virtual ~Erased_Deleter() override {}

            virtual void destroy(T* pointer) override {
                _fn(pointer);
            }

        private:
            FN _fn;
        };

        struct Block {
            T* _pointer;
            // TODO: Atomic refcount?
            i64 _refcount;
            Erased_Deleter_Base* _deleter;

            ~Block() {
                _deleter->destroy(_pointer);
                delete _deleter;
            }
        };

        Block* _block = nullptr;

    public:
        Shared_Ptr() {}

        Shared_Ptr(T* pointer): _block(new Block{pointer, 1, new Erased_Deleter<Default_Deleter<T>>(Default_Deleter<T>())}) {}

        template<typename Deleter>
        Shared_Ptr(T* pointer, Deleter&& deleter): _block(new Block{pointer, 1, new Erased_Deleter<Deleter>(forward<Deleter>(deleter))}) {}

        template<typename... Args>
        Shared_Ptr(Variadic_Construct_Tag tag, Args&&... args): _block(new Block{nullptr, 1, new Erased_Deleter<Default_Deleter<T>>(Default_Deleter<T>())}) {
            _block->_pointer = new T(forward<Args>(args)...);
        }

        template<typename Deleter, typename... Args>
        Shared_Ptr(Deleter&& deleter, Variadic_Construct_Tag tag, Args&&... args)
            : _block(new Block{nullptr, 1, new Erased_Deleter<Deleter>(forward<Deleter>(deleter))}) {
            _block->pointer = new T(forward<Args>(args)...);
        }

        Shared_Ptr(Shared_Ptr const& shared_ptr) {
            if(shared_ptr._block) {
                shared_ptr._block->_refcount += 1;
            }
            _block = shared_ptr._block;
        }

        Shared_Ptr(Shared_Ptr&& shared_ptr): _block(shared_ptr._block) {
            shared_ptr._block = nullptr;
        }

        Shared_Ptr& operator=(Shared_Ptr const& shared_ptr) {
            if(shared_ptr._block) {
                shared_ptr._block->_refcount += 1;
            }
            Block* const old_block = _block;
            _block = shared_ptr._block;
            if(old_block) {
                old_block->_refcount -= 1;
                if(old_block->_refcount == 0) {
                    delete old_block;
                }
            }
            return *this;
        }

        Shared_Ptr& operator=(Shared_Ptr&& shared_ptr) {
            swap(*this, shared_ptr);
            return *this;
        }

        ~Shared_Ptr() {
            if(_block) {
                _block->_refcount -= 1;
                if(_block->_refcount == 0) {
                    delete _block;
                }
            }
        }

        [[nodiscard]] operator bool() const {
            return _block->_pointer;
        }

        [[nodiscard]] T& operator*() const {
            return *_block->_pointer;
        }

        [[nodiscard]] T* operator->() const {
            return _block->_pointer;
        }

        [[nodiscard]] T* get_ptr() const {
            return _block->_pointer;
        }

        [[nodiscard]] i64 get_refcount() const {
            return _block->refcount.load();
        }

        // replace
        // Replace the pointer to the managed object with another one.
        //
        // Returns:
        // Previously owned pointer.
        //
        // TODO: Consider making [[nodiscard]]
        //
        T* replace(T* pointer) {
            T* old = _block->_pointer;
            _block->_pointer = pointer;
            return old;
        }

        friend void swap(Shared_Ptr& lhs, Shared_Ptr& rhs) {
            swap(lhs._block, rhs._block);
        }
    };

    template<typename T>
    [[nodiscard]] bool operator==(Shared_Ptr<T> const& lhs, Shared_Ptr<T> const& rhs) {
        return lhs.get_ptr() == rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator!=(Shared_Ptr<T> const& lhs, Shared_Ptr<T> const& rhs) {
        return lhs.get_ptr() != rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator<(Shared_Ptr<T> const& lhs, Shared_Ptr<T> const& rhs) {
        return lhs.get_ptr() < rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator>(Shared_Ptr<T> const& lhs, Shared_Ptr<T> const& rhs) {
        return lhs.get_ptr() > rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator<=(Shared_Ptr<T> const& lhs, Shared_Ptr<T> const& rhs) {
        return lhs.get_ptr() <= rhs.get_ptr();
    }

    template<typename T>
    [[nodiscard]] bool operator>=(Shared_Ptr<T> const& lhs, Shared_Ptr<T> const& rhs) {
        return lhs.get_ptr() >= rhs.get_ptr();
    }
} // namespace anton
