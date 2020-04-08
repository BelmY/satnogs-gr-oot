# - Try to find nlohmann_json
#
# The following variables are optionally searched for defaults
#  nlohmann_json_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done: 
#  nlohmann_json_FOUND
#  nlohmann_json_INCLUDE_DIRS
#  nlohmann_json_LIBRARIES

include(FindPackageHandleStandardArgs)

set(nlohmann_json_ROOT_DIR "" CACHE PATH "Folder contains nlohmann_json")

find_path(nlohmann_json_INCLUDE_DIR
    NAMES
        nlohmann/json.hpp
        nlohmann/adl_serializer.hpp
        nlohmann/detail/conversions
        nlohmann/detail/conversions/from_json.hpp
        nlohmann/detail/conversions/to_chars.hpp
        nlohmann/detail/conversions/to_json.hpp
        nlohmann/detail/exceptions.hpp
        nlohmann/detail/input/binary_reader.hpp
        nlohmann/detail/input/input_adapters.hpp
        nlohmann/detail/input/json_sax.hpp
        nlohmann/detail/input/lexer.hpp
        nlohmann/detail/input/parser.hpp
        nlohmann/detail/input/position_t.hpp
        nlohmann/detail/iterators/internal_iterator.hpp
        nlohmann/detail/iterators/iter_impl.hpp
        nlohmann/detail/iterators/iteration_proxy.hpp
        nlohmann/detail/iterators/iterator_traits.hpp
        nlohmann/detail/iterators/json_reverse_iterator.hpp
        nlohmann/detail/iterators/primitive_iterator.hpp
        nlohmann/detail/json_pointer.hpp
        nlohmann/detail/json_ref.hpp
        nlohmann/detail/macro_scope.hpp
        nlohmann/detail/macro_unscope.hpp
        nlohmann/detail/meta/cpp_future.hpp
        nlohmann/detail/meta/detected.hpp
        nlohmann/detail/meta/is_sax.hpp
        nlohmann/detail/meta/type_traits.hpp
        nlohmann/detail/meta/void_t.hpp
        nlohmann/detail/output/binary_writer.hpp
        nlohmann/detail/output/output_adapters.hpp
        nlohmann/detail/output/serializer.hpp
        nlohmann/detail/value_t.hpp
        nlohmann/json_fwd.hpp        
    PATHS /usr/local/include
          /usr/include)

set(NLOHMANN_JSON_INCLUDE_DIRS ${nlohmann_json_INCLUDE_DIR})

find_package_handle_standard_args(nlohmann_json DEFAULT_MSG nlohmann_json_INCLUDE_DIR)

if(nlohmann_json_FOUND)
    set(NLOHMANN_JSON_INCLUDE_DIRS ${nlohmann_json_INCLUDE_DIR})
endif()
