# F-Lib

F-Lib是一个Module-Only的STL拓展 & 别名库

## 非功能性拓展
- 注入std::views和std::pmr命名空间

## 功能性拓展
- ### container
  - **string_pool**: 线程安全的字符串池。
  - **any**: 支持移动的std::any。
  - **string_hasher**: 字符串透明哈希

- ### exception
  - **exception**: 使用格式化构造std::runtime_error

- ### memory
  - **size_based_memory_resource**: 分流的内存池资源
  - **polymorphic_dynamic_deleter**: 支持pmr和动态多态的deleter。
  - **unique_ptr**: 使用polymorphic_dynamic_deleter的unique_ptr
  - **make_unique** / **unique_upcast**: 提供了与`std::pmr`兼容的`make_unique`函数和多态上转型的`unique_ptr`转换功能。

- ### rtt (runtime type)
  - **method_not_implemented**: 当类型不支持请求的方法时抛出的异常。
  - **type_t**: 存储类型在运行时的常见行为（默认构造、复制构造、移动构造、析构、复制赋值、移动赋值）。可以转换成std::type_index，支持透明哈希。
  - **is_instance**: 检查对象是否是指定类型。
  - **as**: 将对象转换为指定类型，*保留const和指针修饰*。

