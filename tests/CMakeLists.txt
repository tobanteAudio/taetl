project(etl-tests)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    test_algorithm.cpp
    test_array.cpp
    test_bit.cpp
    test_bitset.cpp
    test_cctype.cpp
    test_charconv.cpp
    test_chrono.cpp
    test_cmath.cpp
    test_cstddef.cpp
    test_cstdint.cpp
    test_cstdio.cpp
    test_cstdlib.cpp
    test_cstring.cpp
    test_concepts.cpp
    test_ctime.cpp
    test_functional.cpp
    test_iterator.cpp
    test_limits.cpp
    test_map.cpp
    test_memory.cpp
    test_mutex.cpp
    test_numbers.cpp
    test_numeric.cpp
    test_optional.cpp
    test_ratio.cpp
    test_scope_guard.cpp
    test_set.cpp
    test_span.cpp
    test_stack.cpp
    test_string.cpp
    test_string_view.cpp
    test_tuple.cpp
    test_type_traits.cpp
    test_utility.cpp
    test_variant.cpp
    test_vector.cpp
    test_version.cpp

    experimental/dsp/test_dsp.cpp
    experimental/format/test_format.cpp

    experimental/net/test_buffer.cpp
    experimental/net/test_byte_order.cpp

    experimental/rtos/test_task.cpp
    experimental/rtos/test_queue.cpp

    experimental/stm32/test_gpio.cpp
    experimental/stm32/test_interrupt.cpp

    experimental/strong_type/test_strong_type.cpp

    experimental/mcp23017/test_mcp23017.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        tobanteAudio::etl
        tobanteAudio::CompilerOptions
        tobanteAudio::CompilerWarnings
        tobanteAudio::CodeCoverage
        Catch2WithMain
)

add_test(NAME unit_tests COMMAND ${PROJECT_NAME})


