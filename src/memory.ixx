//
// Created by MuXin on 2025/11/23.
//

export module f:memory;
export import std;

export namespace f {

using namespace std::pmr;

template<typename SmallPool, typename LargePool, std::size_t Threshold>
class size_based_memory_resource final: public memory_resource {
public:
    explicit
    size_based_memory_resource(memory_resource* upstream=get_default_resource()):
        _small{upstream},
        _large{upstream}
    {}

private:
    void* do_allocate(
        const size_t _Bytes,
        const size_t _Align) override {
        if (_Bytes > Threshold)
            return _large.allocate(_Bytes, _Align);
        return _small.allocate(_Bytes, _Align);
    }
    void do_deallocate(
        void* _Ptr,
        const size_t _Bytes,
        const size_t _Align) override {
        if (_Bytes > Threshold)
            _large.deallocate(_Ptr, _Bytes, _Align);
        _small.deallocate(_Ptr, _Bytes, _Align);
    }
    bool do_is_equal(
        const memory_resource& _That) const noexcept override {
        return this == &_That;
    }

    SmallPool
        _small;
    LargePool
        _large;
};

template<std::size_t Threshold=3 * 1024 * 1024>
using sync_sized_pool = size_based_memory_resource<synchronized_pool_resource, synchronized_pool_resource, Threshold>;

template<std::size_t Threshold=3 * 1024 * 1024>
using sized_pool = size_based_memory_resource<unsynchronized_pool_resource, unsynchronized_pool_resource, Threshold>;


template<typename T>
class polymorphic_dynamic_deleter {
public:
    void operator () (T* ptr) const noexcept {
        ptr->~T();
        _resource->deallocate(ptr, _size, _align);
    }

    explicit
    polymorphic_dynamic_deleter(memory_resource* resource):
    _resource(resource),
    _size{sizeof(T)},
    _align{alignof(T)}{}


    template<typename U>
    requires std::derived_from<T, U>
    explicit
    polymorphic_dynamic_deleter(const polymorphic_dynamic_deleter<U>& deleter):
        _resource(deleter._resource),
        _size{deleter._size},
        _align{deleter._align}
    {}
private:
    memory_resource*
        _resource;
    std::size_t
        _size;
    std::size_t
        _align;
};

template<typename T>
using unique_ptr = std::unique_ptr<T, polymorphic_dynamic_deleter<T>>;

template<typename T, typename... Args>
auto make_unique(memory_resource* mem_res, Args&&... args) noexcept(noexcept(T{std::declval<Args>()...})) {
    const auto ptr = mem_res->allocate(sizeof(T), alignof(T));
    return unique_ptr<T>{
        new (ptr) T {std::forward<Args>(args)...},
        polymorphic_dynamic_deleter<T>{mem_res}
    };
}

template<typename B, typename D>
requires std::derived_from<B, D>
auto unique_upcast(std::unique_ptr<D, polymorphic_dynamic_deleter<D>>&& ptr) noexcept {
    return unique_ptr<B>{
        static_cast<B>(ptr.release()),
        polymorphic_dynamic_deleter<B>{ptr.get_deleter()}
    };
}

}