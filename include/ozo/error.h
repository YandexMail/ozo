#pragma once

#include <ozo/detail/base36.h>

#include <boost/hana/tuple.hpp>
#include <boost/hana/fold.hpp>
#include <boost/asio/error.hpp>
#include <boost/system/system_error.hpp>

/**
 * @defgroup group-errors Errors system
 * @brief Errors system and codes description
 */
namespace ozo {

namespace hana = ::boost::hana;

/**
 * @ingroup group-errors
 * @brief Error code representation of the library.
 *
 * In this library the
 * <a href="https://www.boost.org/doc/libs/1_66_0/libs/system/doc/index.html">Boost.System</a>'s
 * error_code is used. The reason is what it is provided by the
 * <a href="https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/using.html">
 * Boost.Asio</a> library. So the related definitions look like this:
 *
 * @code
using error_code = boost::system::error_code;
using system_error = boost::system::system_error;
using error_category = boost::system::error_category;
using error_condition = boost::system::error_condition;
 * @endcode
 *
 * @note This may be configurable in the future and `std::error_code` support may be added.
 * @sa system_error, error_category, error_condition
 */
using error_code = boost::system::error_code;

/**
 * @ingroup group-errors
 * @brief Error code contaning exception of the library.
 * @sa error_code , error_category, error_condition
 */
using system_error = boost::system::system_error;
/**
 * @ingroup group-errors
 * @brief Error category representation of the library.
 * @sa error_code, system_error, error_condition
 */
using error_category = boost::system::error_category;
/**
 * @ingroup group-errors
 * @brief Error condition representation of the library.
 * @sa error_code, system_error, error_category
 */
using error_condition = boost::system::error_condition;

/**
 * @defgroup group-errors-errors error
 * @brief OZO related errors
 *
 * @ingroup group-errors
 */
namespace error {

/**
 * @brief OZO related error codes
 *
 * Enumeration of error codes provided by the OZO library. Mainly it contains errors related
 * to underlying libpq functions errors, data reflection and so on. In most cases the additional context
 * may be acquired with `error_message()` and `get_error_context()` functions.
 * @ingroup group-errors-errors
 */
enum code {
    ok, //!< a place holder to do not use error code 0
    pq_connection_start_failed, //!< libpq PQConnectionStart function failed
    pq_socket_failed, //!< libpq PQSocket returned -1 as fd
    pq_connection_status_bad, //!< libpq PQstatus returned CONNECTION_BAD
    pq_connect_poll_failed, //!< libpq PQconnectPoll function failed
    oid_type_mismatch, //!< no conversion possible from oid to user-supplied type - the type what user expected is not the same as one which is received from a database
    unexpected_eof, //!< unexpected EOF while data read - something wrong with incoming data from database, data ends unexpectedly
    pg_send_query_params_failed, //!< libpq PQsendQueryParams function failed
    pg_consume_input_failed, //!< libpq PQconsumeInput function failed
    pg_set_nonblocking_failed, //!< libpq PQsetnonblocking function failed
    pg_flush_failed, //!< libpq PQflush function failed
    bad_result_process, //!< error while processing or converting result from the database
    no_sql_state_found, //!< no sql state has been found in database query execution error reply
    result_status_unexpected, //!< got unexpected status from query result
    result_status_empty_query, //!< the string sent to the server was empty
    result_status_bad_response, //!< the server's response was not understood
    oid_request_failed, //!< error during request oids from a database
    bad_object_size, //!< an object size received does not equal to the expected
    bad_array_size, //!< an array size received does not equal to the expected or not supported by the type
    bad_array_dimension, //!< an array dimension count received does not equal to the expected or not supported by the type
    bad_composite_size, //!< a composite's fields number received does not equal to the expected or not supported by the type
    pq_cancel_failed, //!< libpq PQcancel function call failed, see `get_error_context()` for more information
    pq_get_cancel_failed, //!< libpq PQgetCancel function call failed, see `get_error_context()` for more information
};

/**
 * @brief OZO related errors category
 *
 * OZO related errors category object is used to construct error_code.
 *
 * @return `const error_category&` --- reference to the category object
 * @ingroup group-errors-errors
 */
const error_category& category() noexcept;

} // namespace error

/**
 * @defgroup group-errors-sqlstate sqlstate
 * @brief SQL state related errors and conditions
 *
 * @ingroup group-errors
 */
namespace sqlstate {

/**
* @brief SQL state error conditions.
*
* This is a set of error conditions. It may not be complete, since new versions
* of PostgreSQL may add another sql state codes and users may create new ones inside
* a DB logic. So this set may be used to match most of sql states but not all.
*
* Here are list of error conditions related to PostgreSQL error classes.
* A full list of codes can be found in the
* <a href="https://www.postgresql.org/docs/11/errcodes-appendix.html">
* official documentation for PostgreSQL</a>.
* @ingroup group-errors-sqlstate
*/
enum code {
    successful_completion = 0, //!< Class 00 — Successful Completion
    warning = 46656, //!< Class 01 — Warning
    dynamic_result_sets_returned = 46668, // 0100C
    implicit_zero_bit_padding = 46664, // 01008
    null_value_eliminated_in_set_function = 46659, // 01003
    privilege_not_granted = 46663, // 01007
    privilege_not_revoked = 46662, // 01006
    string_data_right_truncation_warning = 46660, // 01004
    deprecated_feature = 79057, // 01P01
    no_data = 93312, //!< Class 02 — No Data (this is also a warning class per the SQL standard)
    no_additional_dynamic_result_sets_returned = 93313, // 02001
    sql_statement_not_yet_complete = 139968, //!< Class 03 — SQL Statement Not Yet Complete
    connection_exception = 373248, //!< Class 08 — Connection Exception
    connection_does_not_exist = 373251, // 08003
    connection_failure = 373254, // 08006
    sqlclient_unable_to_establish_sqlconnection = 373249, // 08001
    sqlserver_rejected_establishment_of_sqlconnection = 373252, // 08004
    transaction_resolution_unknown = 373255, // 08007
    protocol_violation = 405649, // 08P01
    triggered_action_exception = 419904, //!< Class 09 — Triggered Action Exception
    feature_not_supported = 466560, //!< Class 0A — Feature Not Supported
    invalid_transaction_initiation = 513216, //!< Class 0B — Invalid Transaction Initiation
    locator_exception = 699840, //!< Class 0F — Locator Exception
    invalid_locator_specification = 699841, // 0F001
    invalid_grantor = 979776, //!< Class 0L — Invalid Grantor
    invalid_grant_operation = 1012177, // 0LP01
    invalid_role_specification = 1166400, //!< Class 0P — Invalid Role Specification
    diagnostics_exception = 1632960, //!< Class 0Z — Diagnostics Exception
    stacked_diagnostics_accessed_without_active_handler = 1632962, // 0Z002
    case_not_found = 3359232, //!< Class 20 — Case Not Found
    cardinality_violation = 3405888, //!< Class 21 — Cardinality Violation
    data_exception = 3452544, //!< Class 22 — Data Exception
    array_subscript_error = 3452630, // 2202E
    character_not_in_repertoire = 3452617, // 22021
    datetime_field_overflow = 3452552, // 22008
    division_by_zero = 3452582, // 22012
    error_in_assignment = 3452549, // 22005
    escape_character_conflict = 3452555, // 2200B
    indicator_overflow = 3452618, // 22022
    interval_field_overflow = 3452585, // 22015
    invalid_argument_for_logarithm = 3452594, // 2201E
    invalid_argument_for_ntile_function = 3452584, // 22014
    invalid_argument_for_nth_value_function = 3452586, // 22016
    invalid_argument_for_power_function = 3452595, // 2201F
    invalid_argument_for_width_bucket_function = 3452596, // 2201G
    invalid_character_value_for_cast = 3452588, // 22018
    invalid_datetime_format = 3452551, // 22007
    invalid_escape_character = 3452589, // 22019
    invalid_escape_octet = 3452557, // 2200D
    invalid_escape_sequence = 3452621, // 22025
    nonstandard_use_of_escape_character = 3484950, // 22P06
    invalid_indicator_parameter_value = 3452580, // 22010
    invalid_parameter_value = 3452619, // 22023
    invalid_regular_expression = 3452591, // 2201B
    invalid_row_count_in_limit_clause = 3452612, // 2201W
    invalid_row_count_in_result_offset_clause = 3452613, // 2201X
    invalid_time_zone_displacement_value = 3452553, // 22009
    invalid_use_of_escape_character = 3452556, // 2200C
    most_specific_type_mismatch = 3452560, // 2200G
    null_value_not_allowed = 3452548, // 22004
    null_value_no_indicator_parameter = 3452546, // 22002
    numeric_value_out_of_range = 3452547, // 22003
    string_data_length_mismatch = 3452622, // 22026
    string_data_right_truncation = 3452545, // 22001
    substring_error = 3452581, // 22011
    trim_error = 3452623, // 22027
    unterminated_c_string = 3452620, // 22024
    zero_length_character_string = 3452559, // 2200F
    floating_point_exception = 3484945, // 22P01
    invalid_text_representation = 3484946, // 22P02
    invalid_binary_representation = 3484947, // 22P03
    bad_copy_file_format = 3484948, // 22P04
    untranslatable_character = 3484949, // 22P05
    not_an_xml_document = 3452565, // 2200L
    invalid_xml_document = 3452566, // 2200M
    invalid_xml_content = 3452567, // 2200N
    invalid_xml_comment = 3452572, // 2200S
    invalid_xml_processing_instruction = 3452573, // 2200T
    integrity_constraint_violation = 3499200, //!< Class 23 — Integrity Constraint Violation
    restrict_violation = 3499201, // 23001
    not_null_violation = 3505682, // 23502
    foreign_key_violation = 3505683, // 23503
    unique_violation = 3505685, // 23505
    check_violation = 3505720, // 23514
    exclusion_violation = 3531601, // 23P01
    invalid_cursor_state = 3545856, //!< Class 24 — Invalid Cursor State
    invalid_transaction_state = 3592512, //!< Class 25 — Invalid Transaction State
    active_sql_transaction = 3592513, // 25001
    branch_transaction_already_active = 3592514, // 25002
    held_cursor_requires_same_isolation_level = 3592520, // 25008
    inappropriate_access_mode_for_branch_transaction = 3592515, // 25003
    inappropriate_isolation_level_for_branch_transaction = 3592516, // 25004
    no_active_sql_transaction_for_branch_transaction = 3592517, // 25005
    read_only_sql_transaction = 3592518, // 25006
    schema_and_data_statement_mixing_not_supported = 3592519, // 25007
    no_active_sql_transaction = 3624913, // 25P01
    in_failed_sql_transaction = 3624914, // 25P02
    invalid_sql_statement_name = 3639168, //!< Class 26 — Invalid SQL Statement Name
    triggered_data_change_violation = 3685824, //!< Class 27 — Triggered Data Change Violation
    invalid_authorization_specification = 3732480, //!< Class 28 — Invalid Authorization Specification
    invalid_password = 3764881, // 28P01
    dependent_privilege_descriptors_still_exist = 3872448, //!< Class 2B — Dependent Privilege Descriptors Still Exist
    dependent_objects_still_exist = 3904849, // 2BP01
    invalid_transaction_termination = 3965760, //!< Class 2D — Invalid Transaction Termination
    sql_routine_exception = 4059072, //!< Class 2F — SQL Routine Exception
    function_executed_no_return_statement = 4059077, // 2F005
    modifying_sql_data_not_permitted = 4059074, // 2F002
    prohibited_sql_statement_attempted = 4059075, // 2F003
    reading_sql_data_not_permitted = 4059076, // 2F004
    invalid_cursor_name = 5225472, //!< Class 34 — Invalid Cursor Name
    external_routine_exception = 5412096, //!< Class 38 — External Routine Exception
    containing_sql_not_permitted = 5412097, // 38001
    modifying_sql_data_not_permitted_external = 5412098, // 38002
    prohibited_sql_statement_attempted_external = 5412099, // 38003
    reading_sql_data_not_permitted_external = 5412100, // 38004
    external_routine_invocation_exception = 5458752, //!< Class 39 — External Routine Invocation Exception
    invalid_sqlstate_returned = 5458753, // 39001
    null_value_not_allowed_external = 5458756, // 39004
    trigger_protocol_violated = 5491153, // 39P01
    srf_protocol_violated = 5491154, // 39P02
    savepoint_exception = 5552064, //!< Class 3B — Savepoint Exception
    invalid_savepoint_specification = 5552065, // 3B001
    invalid_catalog_name = 5645376, //!< Class 3D — Invalid Catalog Name
    invalid_schema_name = 5738688, //!< Class 3F — Invalid Schema Name
    transaction_rollback = 6718464, //!< Class 40 — Transaction Rollback
    transaction_integrity_constraint_violation = 6718466, // 40002
    serialization_failure = 6718465, // 40001
    statement_completion_unknown = 6718467, // 40003
    deadlock_detected = 6750865, // 40P01
    syntax_error_or_access_rule_violation = 6811776, //!< Class 42 — Syntax Error or Access Rule Violation
    syntax_error = 6819553, // 42601
    insufficient_privilege = 6818257, // 42501
    cannot_coerce = 6822294, // 42846
    grouping_error = 6822147, // 42803
    windowing_error = 6844248, // 42P20
    invalid_recursion = 6844221, // 42P19
    invalid_foreign_key = 6822252, // 42830
    invalid_name = 6819554, // 42602
    name_too_long = 6819626, // 42622
    reserved_name = 6823557, // 42939
    datatype_mismatch = 6822148, // 42804
    indeterminate_datatype = 6844220, // 42P18
    collation_mismatch = 6844249, // 42P21
    indeterminate_collation = 6844250, // 42P22
    wrong_object_type = 6822153, // 42809
    undefined_column = 6820851, // 42703
    undefined_function = 6822435, // 42883
    undefined_table = 6844177, // 42P01
    undefined_parameter = 6844178, // 42P02
    undefined_object = 6820852, // 42704
    duplicate_column = 6820849, // 42701
    duplicate_cursor = 6844179, // 42P03
    duplicate_database = 6844180, // 42P04
    duplicate_function = 6820923, // 42723
    duplicate_prepared_statement = 6844181, // 42P05
    duplicate_schema = 6844182, // 42P06
    duplicate_table = 6844183, // 42P07
    duplicate_alias = 6820886, // 42712
    duplicate_object = 6820884, // 42710
    ambiguous_column = 6820850, // 42702
    ambiguous_function = 6820925, // 42725
    ambiguous_parameter = 6844184, // 42P08
    ambiguous_alias = 6844185, // 42P09
    invalid_column_reference = 6844212, // 42P10
    invalid_column_definition = 6819589, // 42611
    invalid_cursor_definition = 6844213, // 42P11
    invalid_database_definition = 6844214, // 42P12
    invalid_function_definition = 6844215, // 42P13
    invalid_prepared_statement_definition = 6844216, // 42P14
    invalid_schema_definition = 6844217, // 42P15
    invalid_table_definition = 6844218, // 42P16
    invalid_object_definition = 6844219, // 42P17
    with_check_option_violation = 6905088, //!< Class 44 — WITH CHECK OPTION Violation
    insufficient_resources = 8538048, //!< Class 53 — Insufficient Resources
    disk_full = 8539344, // 53100
    out_of_memory = 8540640, // 53200
    too_many_connections = 8541936, // 53300
    configuration_limit_exceeded = 8543232, // 53400
    program_limit_exceeded = 8584704, //!< Class 54 — Program Limit Exceeded
    statement_too_complex = 8584705, // 54001
    too_many_columns = 8584741, // 54011
    too_many_arguments = 8584779, // 54023
    object_not_in_prerequisite_state = 8631360, //!< Class 55 — Object Not In Prerequisite State
    object_in_use = 8631366, // 55006
    cant_change_runtime_param = 8663762, // 55P02
    lock_not_available = 8663763, // 55P03
    operator_intervention = 8724672, //!< Class 57 — Operator Intervention
    query_canceled = 8724712, // 57014
    admin_shutdown = 8757073, // 57P01
    crash_shutdown = 8757074, // 57P02
    cannot_connect_now = 8757075, // 57P03
    database_dropped = 8757076, // 57P04
    system_error = 8771328, //!< Class 58 — System Error (errors external to PostgreSQL itself)
    io_error = 8771436, // 58030
    undefined_file = 8803729, // 58P01
    duplicate_file = 8803730, // 58P02
    snapshot_too_old = 11850624, //!< Class 72 — Snapshot Failure
    config_file_error = 25194240, //!< Class F0 — Configuration File Error
    lock_file_exists = 25194241, // F0001
    fdw_error = 29999808, //!< Class HV — Foreign Data Wrapper Error (SQL/MED)
    fdw_column_name_not_found = 29999813, // HV005
    fdw_dynamic_parameter_value_needed = 29999810, // HV002
    fdw_function_sequence_error = 29999844, // HV010
    fdw_inconsistent_descriptor_information = 29999881, // HV021
    fdw_invalid_attribute_value = 29999884, // HV024
    fdw_invalid_column_name = 29999815, // HV007
    fdw_invalid_column_number = 29999816, // HV008
    fdw_invalid_data_type = 29999812, // HV004
    fdw_invalid_data_type_descriptors = 29999814, // HV006
    fdw_invalid_descriptor_field_identifier = 30000133, // HV091
    fdw_invalid_handle = 29999819, // HV00B
    fdw_invalid_option_index = 29999820, // HV00C
    fdw_invalid_option_name = 29999821, // HV00D
    fdw_invalid_string_length_or_buffer_length = 30000132, // HV090
    fdw_invalid_string_format = 29999818, // HV00A
    fdw_invalid_use_of_null_pointer = 29999817, // HV009
    fdw_too_many_handles = 29999848, // HV014
    fdw_out_of_memory = 29999809, // HV001
    fdw_no_schemas = 29999833, // HV00P
    fdw_option_name_not_found = 29999827, // HV00J
    fdw_reply_handle = 29999828, // HV00K
    fdw_schema_not_found = 29999834, // HV00Q
    fdw_table_not_found = 29999835, // HV00R
    fdw_unable_to_create_execution = 29999829, // HV00L
    fdw_unable_to_create_reply = 29999830, // HV00M
    fdw_unable_to_establish_connection = 29999831, // HV00N
    plpgsql_error = 41990400, //!< Class P0 — PL/pgSQL Error
    raise_exception = 41990401, // P0001
    no_data_found = 41990402, // P0002
    too_many_rows = 41990403, // P0003
    internal_error = 56966976, //!< Class XX — Internal Error
    data_corrupted = 56966977, // XX001
    index_corrupted = 56966978, // XX002
};

/**
 * @brief Sql state related errors category.
 *
 * @return `const error_category&` --- reference to the category object
 * @ingroup group-errors-sqlstate
 */
const error_category& category() noexcept;

} // namespace sqlstate

/**
 * @defgroup group-errors-errc errc
 * @brief Useful error conditions
 *
 * @ingroup group-errors
 */
namespace errc {

/**
 * @brief Error conditions of the library
 *
 * This useful error conditions incorporate ozo, libpq and Boost.Asio error codes.
 *
 * @note Error condition `type_mismatch` for custom types may occur in case of database schema update which may lead to
 * custom type oid change. For built-in types it signals about mismatching.
 * @ingroup group-errors-errc
 */
enum code {
    ok, //!< no error placeholder
    connection_error, //!< connection-related error condition, incorporates ozo, libpq and Boost.Asio connection errors
    database_readonly, //!< database in read-only state - useful to detect attempt of modify data on replica host
    introspection_error, //!< errors related to objects serialization/deserialization
    type_mismatch, //!< result type mismatch, indicates types mismatch between result of query and expected result
    protocol_error, //!< specific protocol-related errors
};

/**
 * @brief Error conditions category
 *
 * Error conditions category object is used to construct error_code.
 *
 * @return `const error_category&` --- reference to the category object
 * @ingroup group-errors-errc
 */
const error_category& category() noexcept;

} // namespace errc

} // namespace ozo

namespace boost {
namespace system {

template <>
struct is_error_code_enum<ozo::error::code> : std::true_type {};

template <>
struct is_error_condition_enum<ozo::sqlstate::code> : std::true_type {};

template <>
struct is_error_condition_enum<ozo::errc::code> : std::true_type {};

} // namespace system
} // namespace boost

namespace ozo {
namespace error {

inline auto make_error_code(const code e) {
    return error_code(static_cast<int>(e), category());
}

namespace impl {

class category final : public error_category {
public:
    const char* name() const noexcept override final { return "ozo::error::category"; }

    std::string message(int value) const override final {
        switch (code(value)) {
            case ok:
                return "no error";
            case pq_connection_start_failed:
                return "pq_connection_start_failed - PQConnectionStart function failed";
            case pq_socket_failed:
                return "pq_socket_failed - PQSocket returned -1 as fd - it seems like there is no connection";
            case pq_connection_status_bad:
                return "pq_connection_status_bad - PQstatus returned CONNECTION_BAD";
            case pq_connect_poll_failed:
                return "pq_connect_poll_failed - PQconnectPoll function failed";
            case oid_type_mismatch:
                return "no conversion possible from oid to user-supplied type";
            case unexpected_eof:
                return "unexpected EOF while data read";
            case pg_send_query_params_failed:
                return "pg_send_query_params_failed - PQsendQueryParams function failed";
            case pg_consume_input_failed:
                return "pg_consume_input_failed - PQconsumeInput function failed";
            case pg_set_nonblocking_failed:
                return "pg_set_nonblocking_failed - PQsetnonblocking function failed";
            case pg_flush_failed:
                return "pg_flush_failed - PQflush function failed";
            case bad_result_process:
                return "bad_result_process - error while processing or converting result from the database";
            case no_sql_state_found:
                return "no_sql_state_found - no sql state has been found in database query execution error reply";
            case result_status_unexpected:
                return "result_status_unexpected - got unexpected status from query result";
            case result_status_empty_query:
                return "result_status_empty_query - the string sent to the server was empty";
            case result_status_bad_response:
                return "result_status_bad_response - the server's response was not understood";
            case oid_request_failed:
                return "error during request oids from a database";
            case bad_object_size:
                return "an object size received does not equal to the expected";
            case bad_array_size:
                return "an array size received does not equal to the expected or not supported by the type";
            case bad_array_dimension:
                return "an array dimension count received does not equal to the expected or not supported by the type";
            case bad_composite_size:
                return "a composite's fields number received does not equal to the expected or not supported by the type";
            case pq_cancel_failed:
                return "libpq PQcancel function call failed";
            case pq_get_cancel_failed:
                return "libpq PQgetCancel function call failed";
        }
        return "no message for value: " + std::to_string(value);
    }
};

} // namespace impl

inline const error_category& category() noexcept {
    static impl::category instance;
    return instance;
}

} // namespace error

namespace sqlstate {

inline auto make_error_code(const int e) {
    return error_code(e, category());
}

inline auto make_error_condition(const code e) {
    return error_condition(static_cast<int>(e), category());
}

namespace impl {

class category final : public error_category {
public:
    const char* name() const noexcept override final { return "ozo::sqlstate::category"; }

    std::string message(int value) const override final {
        #define OZO_SQLSTATE_NAME(value) case value: return boost::hana::to<char const*>(\
                BOOST_HANA_STRING(#value) + BOOST_HANA_STRING("(") +\
                detail::ltob36<value>() + BOOST_HANA_STRING(")"));
        switch (value) {
            OZO_SQLSTATE_NAME(successful_completion)
            OZO_SQLSTATE_NAME(warning)
            OZO_SQLSTATE_NAME(dynamic_result_sets_returned)
            OZO_SQLSTATE_NAME(implicit_zero_bit_padding)
            OZO_SQLSTATE_NAME(null_value_eliminated_in_set_function)
            OZO_SQLSTATE_NAME(privilege_not_granted)
            OZO_SQLSTATE_NAME(privilege_not_revoked)
            OZO_SQLSTATE_NAME(string_data_right_truncation_warning)
            OZO_SQLSTATE_NAME(deprecated_feature)
            OZO_SQLSTATE_NAME(no_data)
            OZO_SQLSTATE_NAME(no_additional_dynamic_result_sets_returned)
            OZO_SQLSTATE_NAME(sql_statement_not_yet_complete)
            OZO_SQLSTATE_NAME(connection_exception)
            OZO_SQLSTATE_NAME(connection_does_not_exist)
            OZO_SQLSTATE_NAME(connection_failure)
            OZO_SQLSTATE_NAME(sqlclient_unable_to_establish_sqlconnection)
            OZO_SQLSTATE_NAME(sqlserver_rejected_establishment_of_sqlconnection)
            OZO_SQLSTATE_NAME(transaction_resolution_unknown)
            OZO_SQLSTATE_NAME(protocol_violation)
            OZO_SQLSTATE_NAME(triggered_action_exception)
            OZO_SQLSTATE_NAME(feature_not_supported)
            OZO_SQLSTATE_NAME(invalid_transaction_initiation)
            OZO_SQLSTATE_NAME(locator_exception)
            OZO_SQLSTATE_NAME(invalid_locator_specification)
            OZO_SQLSTATE_NAME(invalid_grantor)
            OZO_SQLSTATE_NAME(invalid_grant_operation)
            OZO_SQLSTATE_NAME(invalid_role_specification)
            OZO_SQLSTATE_NAME(diagnostics_exception)
            OZO_SQLSTATE_NAME(stacked_diagnostics_accessed_without_active_handler)
            OZO_SQLSTATE_NAME(case_not_found)
            OZO_SQLSTATE_NAME(cardinality_violation)
            OZO_SQLSTATE_NAME(data_exception)
            OZO_SQLSTATE_NAME(array_subscript_error)
            OZO_SQLSTATE_NAME(character_not_in_repertoire)
            OZO_SQLSTATE_NAME(datetime_field_overflow)
            OZO_SQLSTATE_NAME(division_by_zero)
            OZO_SQLSTATE_NAME(error_in_assignment)
            OZO_SQLSTATE_NAME(escape_character_conflict)
            OZO_SQLSTATE_NAME(indicator_overflow)
            OZO_SQLSTATE_NAME(interval_field_overflow)
            OZO_SQLSTATE_NAME(invalid_argument_for_logarithm)
            OZO_SQLSTATE_NAME(invalid_argument_for_ntile_function)
            OZO_SQLSTATE_NAME(invalid_argument_for_nth_value_function)
            OZO_SQLSTATE_NAME(invalid_argument_for_power_function)
            OZO_SQLSTATE_NAME(invalid_argument_for_width_bucket_function)
            OZO_SQLSTATE_NAME(invalid_character_value_for_cast)
            OZO_SQLSTATE_NAME(invalid_datetime_format)
            OZO_SQLSTATE_NAME(invalid_escape_character)
            OZO_SQLSTATE_NAME(invalid_escape_octet)
            OZO_SQLSTATE_NAME(invalid_escape_sequence)
            OZO_SQLSTATE_NAME(nonstandard_use_of_escape_character)
            OZO_SQLSTATE_NAME(invalid_indicator_parameter_value)
            OZO_SQLSTATE_NAME(invalid_parameter_value)
            OZO_SQLSTATE_NAME(invalid_regular_expression)
            OZO_SQLSTATE_NAME(invalid_row_count_in_limit_clause)
            OZO_SQLSTATE_NAME(invalid_row_count_in_result_offset_clause)
            OZO_SQLSTATE_NAME(invalid_time_zone_displacement_value)
            OZO_SQLSTATE_NAME(invalid_use_of_escape_character)
            OZO_SQLSTATE_NAME(most_specific_type_mismatch)
            OZO_SQLSTATE_NAME(null_value_not_allowed)
            OZO_SQLSTATE_NAME(null_value_no_indicator_parameter)
            OZO_SQLSTATE_NAME(numeric_value_out_of_range)
            OZO_SQLSTATE_NAME(string_data_length_mismatch)
            OZO_SQLSTATE_NAME(string_data_right_truncation)
            OZO_SQLSTATE_NAME(substring_error)
            OZO_SQLSTATE_NAME(trim_error)
            OZO_SQLSTATE_NAME(unterminated_c_string)
            OZO_SQLSTATE_NAME(zero_length_character_string)
            OZO_SQLSTATE_NAME(floating_point_exception)
            OZO_SQLSTATE_NAME(invalid_text_representation)
            OZO_SQLSTATE_NAME(invalid_binary_representation)
            OZO_SQLSTATE_NAME(bad_copy_file_format)
            OZO_SQLSTATE_NAME(untranslatable_character)
            OZO_SQLSTATE_NAME(not_an_xml_document)
            OZO_SQLSTATE_NAME(invalid_xml_document)
            OZO_SQLSTATE_NAME(invalid_xml_content)
            OZO_SQLSTATE_NAME(invalid_xml_comment)
            OZO_SQLSTATE_NAME(invalid_xml_processing_instruction)
            OZO_SQLSTATE_NAME(integrity_constraint_violation)
            OZO_SQLSTATE_NAME(restrict_violation)
            OZO_SQLSTATE_NAME(not_null_violation)
            OZO_SQLSTATE_NAME(foreign_key_violation)
            OZO_SQLSTATE_NAME(unique_violation)
            OZO_SQLSTATE_NAME(check_violation)
            OZO_SQLSTATE_NAME(exclusion_violation)
            OZO_SQLSTATE_NAME(invalid_cursor_state)
            OZO_SQLSTATE_NAME(invalid_transaction_state)
            OZO_SQLSTATE_NAME(active_sql_transaction)
            OZO_SQLSTATE_NAME(branch_transaction_already_active)
            OZO_SQLSTATE_NAME(held_cursor_requires_same_isolation_level)
            OZO_SQLSTATE_NAME(inappropriate_access_mode_for_branch_transaction)
            OZO_SQLSTATE_NAME(inappropriate_isolation_level_for_branch_transaction)
            OZO_SQLSTATE_NAME(no_active_sql_transaction_for_branch_transaction)
            OZO_SQLSTATE_NAME(read_only_sql_transaction)
            OZO_SQLSTATE_NAME(schema_and_data_statement_mixing_not_supported)
            OZO_SQLSTATE_NAME(no_active_sql_transaction)
            OZO_SQLSTATE_NAME(in_failed_sql_transaction)
            OZO_SQLSTATE_NAME(invalid_sql_statement_name)
            OZO_SQLSTATE_NAME(triggered_data_change_violation)
            OZO_SQLSTATE_NAME(invalid_authorization_specification)
            OZO_SQLSTATE_NAME(invalid_password)
            OZO_SQLSTATE_NAME(dependent_privilege_descriptors_still_exist)
            OZO_SQLSTATE_NAME(dependent_objects_still_exist)
            OZO_SQLSTATE_NAME(invalid_transaction_termination)
            OZO_SQLSTATE_NAME(sql_routine_exception)
            OZO_SQLSTATE_NAME(function_executed_no_return_statement)
            OZO_SQLSTATE_NAME(modifying_sql_data_not_permitted)
            OZO_SQLSTATE_NAME(prohibited_sql_statement_attempted)
            OZO_SQLSTATE_NAME(reading_sql_data_not_permitted)
            OZO_SQLSTATE_NAME(invalid_cursor_name)
            OZO_SQLSTATE_NAME(external_routine_exception)
            OZO_SQLSTATE_NAME(containing_sql_not_permitted)
            OZO_SQLSTATE_NAME(modifying_sql_data_not_permitted_external)
            OZO_SQLSTATE_NAME(prohibited_sql_statement_attempted_external)
            OZO_SQLSTATE_NAME(reading_sql_data_not_permitted_external)
            OZO_SQLSTATE_NAME(external_routine_invocation_exception)
            OZO_SQLSTATE_NAME(invalid_sqlstate_returned)
            OZO_SQLSTATE_NAME(null_value_not_allowed_external)
            OZO_SQLSTATE_NAME(trigger_protocol_violated)
            OZO_SQLSTATE_NAME(srf_protocol_violated)
            OZO_SQLSTATE_NAME(savepoint_exception)
            OZO_SQLSTATE_NAME(invalid_savepoint_specification)
            OZO_SQLSTATE_NAME(invalid_catalog_name)
            OZO_SQLSTATE_NAME(invalid_schema_name)
            OZO_SQLSTATE_NAME(transaction_rollback)
            OZO_SQLSTATE_NAME(transaction_integrity_constraint_violation)
            OZO_SQLSTATE_NAME(serialization_failure)
            OZO_SQLSTATE_NAME(statement_completion_unknown)
            OZO_SQLSTATE_NAME(deadlock_detected)
            OZO_SQLSTATE_NAME(syntax_error_or_access_rule_violation)
            OZO_SQLSTATE_NAME(syntax_error)
            OZO_SQLSTATE_NAME(insufficient_privilege)
            OZO_SQLSTATE_NAME(cannot_coerce)
            OZO_SQLSTATE_NAME(grouping_error)
            OZO_SQLSTATE_NAME(windowing_error)
            OZO_SQLSTATE_NAME(invalid_recursion)
            OZO_SQLSTATE_NAME(invalid_foreign_key)
            OZO_SQLSTATE_NAME(invalid_name)
            OZO_SQLSTATE_NAME(name_too_long)
            OZO_SQLSTATE_NAME(reserved_name)
            OZO_SQLSTATE_NAME(datatype_mismatch)
            OZO_SQLSTATE_NAME(indeterminate_datatype)
            OZO_SQLSTATE_NAME(collation_mismatch)
            OZO_SQLSTATE_NAME(indeterminate_collation)
            OZO_SQLSTATE_NAME(wrong_object_type)
            OZO_SQLSTATE_NAME(undefined_column)
            OZO_SQLSTATE_NAME(undefined_function)
            OZO_SQLSTATE_NAME(undefined_table)
            OZO_SQLSTATE_NAME(undefined_parameter)
            OZO_SQLSTATE_NAME(undefined_object)
            OZO_SQLSTATE_NAME(duplicate_column)
            OZO_SQLSTATE_NAME(duplicate_cursor)
            OZO_SQLSTATE_NAME(duplicate_database)
            OZO_SQLSTATE_NAME(duplicate_function)
            OZO_SQLSTATE_NAME(duplicate_prepared_statement)
            OZO_SQLSTATE_NAME(duplicate_schema)
            OZO_SQLSTATE_NAME(duplicate_table)
            OZO_SQLSTATE_NAME(duplicate_alias)
            OZO_SQLSTATE_NAME(duplicate_object)
            OZO_SQLSTATE_NAME(ambiguous_column)
            OZO_SQLSTATE_NAME(ambiguous_function)
            OZO_SQLSTATE_NAME(ambiguous_parameter)
            OZO_SQLSTATE_NAME(ambiguous_alias)
            OZO_SQLSTATE_NAME(invalid_column_reference)
            OZO_SQLSTATE_NAME(invalid_column_definition)
            OZO_SQLSTATE_NAME(invalid_cursor_definition)
            OZO_SQLSTATE_NAME(invalid_database_definition)
            OZO_SQLSTATE_NAME(invalid_function_definition)
            OZO_SQLSTATE_NAME(invalid_prepared_statement_definition)
            OZO_SQLSTATE_NAME(invalid_schema_definition)
            OZO_SQLSTATE_NAME(invalid_table_definition)
            OZO_SQLSTATE_NAME(invalid_object_definition)
            OZO_SQLSTATE_NAME(with_check_option_violation)
            OZO_SQLSTATE_NAME(insufficient_resources)
            OZO_SQLSTATE_NAME(disk_full)
            OZO_SQLSTATE_NAME(out_of_memory)
            OZO_SQLSTATE_NAME(too_many_connections)
            OZO_SQLSTATE_NAME(configuration_limit_exceeded)
            OZO_SQLSTATE_NAME(program_limit_exceeded)
            OZO_SQLSTATE_NAME(statement_too_complex)
            OZO_SQLSTATE_NAME(too_many_columns)
            OZO_SQLSTATE_NAME(too_many_arguments)
            OZO_SQLSTATE_NAME(object_not_in_prerequisite_state)
            OZO_SQLSTATE_NAME(object_in_use)
            OZO_SQLSTATE_NAME(cant_change_runtime_param)
            OZO_SQLSTATE_NAME(lock_not_available)
            OZO_SQLSTATE_NAME(operator_intervention)
            OZO_SQLSTATE_NAME(query_canceled)
            OZO_SQLSTATE_NAME(admin_shutdown)
            OZO_SQLSTATE_NAME(crash_shutdown)
            OZO_SQLSTATE_NAME(cannot_connect_now)
            OZO_SQLSTATE_NAME(database_dropped)
            OZO_SQLSTATE_NAME(system_error)
            OZO_SQLSTATE_NAME(io_error)
            OZO_SQLSTATE_NAME(undefined_file)
            OZO_SQLSTATE_NAME(duplicate_file)
            OZO_SQLSTATE_NAME(config_file_error)
            OZO_SQLSTATE_NAME(lock_file_exists)
            OZO_SQLSTATE_NAME(fdw_error)
            OZO_SQLSTATE_NAME(fdw_column_name_not_found)
            OZO_SQLSTATE_NAME(fdw_dynamic_parameter_value_needed)
            OZO_SQLSTATE_NAME(fdw_function_sequence_error)
            OZO_SQLSTATE_NAME(fdw_inconsistent_descriptor_information)
            OZO_SQLSTATE_NAME(fdw_invalid_attribute_value)
            OZO_SQLSTATE_NAME(fdw_invalid_column_name)
            OZO_SQLSTATE_NAME(fdw_invalid_column_number)
            OZO_SQLSTATE_NAME(fdw_invalid_data_type)
            OZO_SQLSTATE_NAME(fdw_invalid_data_type_descriptors)
            OZO_SQLSTATE_NAME(fdw_invalid_descriptor_field_identifier)
            OZO_SQLSTATE_NAME(fdw_invalid_handle)
            OZO_SQLSTATE_NAME(fdw_invalid_option_index)
            OZO_SQLSTATE_NAME(fdw_invalid_option_name)
            OZO_SQLSTATE_NAME(fdw_invalid_string_length_or_buffer_length)
            OZO_SQLSTATE_NAME(fdw_invalid_string_format)
            OZO_SQLSTATE_NAME(fdw_invalid_use_of_null_pointer)
            OZO_SQLSTATE_NAME(fdw_too_many_handles)
            OZO_SQLSTATE_NAME(fdw_out_of_memory)
            OZO_SQLSTATE_NAME(fdw_no_schemas)
            OZO_SQLSTATE_NAME(fdw_option_name_not_found)
            OZO_SQLSTATE_NAME(fdw_reply_handle)
            OZO_SQLSTATE_NAME(fdw_schema_not_found)
            OZO_SQLSTATE_NAME(fdw_table_not_found)
            OZO_SQLSTATE_NAME(fdw_unable_to_create_execution)
            OZO_SQLSTATE_NAME(fdw_unable_to_create_reply)
            OZO_SQLSTATE_NAME(fdw_unable_to_establish_connection)
            OZO_SQLSTATE_NAME(plpgsql_error)
            OZO_SQLSTATE_NAME(raise_exception)
            OZO_SQLSTATE_NAME(no_data_found)
            OZO_SQLSTATE_NAME(too_many_rows)
            OZO_SQLSTATE_NAME(internal_error)
            OZO_SQLSTATE_NAME(data_corrupted)
            OZO_SQLSTATE_NAME(index_corrupted)
        }
    #undef OZO_SQLSTATE_NAME
        return "sql state " + detail::ltob36(value);
    }

    static constexpr int class_width = 46656; //<! Error code class width 01000 in base 36

    bool equivalent (const error_code& code, int condition) const noexcept override final {
        if (code.category() == *this) {
            const auto value_in = [v = code.value()](enum code cond) {
                return v >= cond && v < cond + class_width;
            };
#define OZO_SQL_STATE_CONDITION(name) case name: return value_in(name)
            switch(condition) {
                OZO_SQL_STATE_CONDITION(warning);
                OZO_SQL_STATE_CONDITION(no_data);
                OZO_SQL_STATE_CONDITION(sql_statement_not_yet_complete);
                OZO_SQL_STATE_CONDITION(connection_exception);
                OZO_SQL_STATE_CONDITION(triggered_action_exception);
                OZO_SQL_STATE_CONDITION(feature_not_supported);
                OZO_SQL_STATE_CONDITION(invalid_transaction_initiation);
                OZO_SQL_STATE_CONDITION(locator_exception);
                OZO_SQL_STATE_CONDITION(invalid_grantor);
                OZO_SQL_STATE_CONDITION(invalid_role_specification);
                OZO_SQL_STATE_CONDITION(diagnostics_exception);
                OZO_SQL_STATE_CONDITION(case_not_found);
                OZO_SQL_STATE_CONDITION(cardinality_violation);
                OZO_SQL_STATE_CONDITION(data_exception);
                OZO_SQL_STATE_CONDITION(integrity_constraint_violation);
                OZO_SQL_STATE_CONDITION(invalid_cursor_state);
                OZO_SQL_STATE_CONDITION(invalid_transaction_state);
                OZO_SQL_STATE_CONDITION(invalid_sql_statement_name);
                OZO_SQL_STATE_CONDITION(triggered_data_change_violation);
                OZO_SQL_STATE_CONDITION(invalid_authorization_specification);
                OZO_SQL_STATE_CONDITION(dependent_privilege_descriptors_still_exist);
                OZO_SQL_STATE_CONDITION(invalid_transaction_termination);
                OZO_SQL_STATE_CONDITION(sql_routine_exception);
                OZO_SQL_STATE_CONDITION(invalid_cursor_name);
                OZO_SQL_STATE_CONDITION(external_routine_exception);
                OZO_SQL_STATE_CONDITION(external_routine_invocation_exception);
                OZO_SQL_STATE_CONDITION(savepoint_exception);
                OZO_SQL_STATE_CONDITION(invalid_catalog_name);
                OZO_SQL_STATE_CONDITION(invalid_schema_name);
                OZO_SQL_STATE_CONDITION(transaction_rollback);
                OZO_SQL_STATE_CONDITION(syntax_error_or_access_rule_violation);
                OZO_SQL_STATE_CONDITION(with_check_option_violation);
                OZO_SQL_STATE_CONDITION(insufficient_resources);
                OZO_SQL_STATE_CONDITION(program_limit_exceeded);
                OZO_SQL_STATE_CONDITION(object_not_in_prerequisite_state);
                OZO_SQL_STATE_CONDITION(operator_intervention);
                OZO_SQL_STATE_CONDITION(system_error);
                OZO_SQL_STATE_CONDITION(snapshot_too_old);
                OZO_SQL_STATE_CONDITION(config_file_error);
                OZO_SQL_STATE_CONDITION(fdw_error);
                OZO_SQL_STATE_CONDITION(plpgsql_error);
                OZO_SQL_STATE_CONDITION(internal_error);
            }
        }
        return error_category::equivalent(code, condition);
#undef OZO_SQL_STATE_CONDITION
    }
};

} // namespace impl

inline const error_category& category() noexcept {
    static impl::category instance;
    return instance;
}

} // namespace sqlstate

namespace errc {

inline auto make_error_condition(const code e) {
    return error_condition(static_cast<int>(e), category());
}

template<typename Conditions>
constexpr bool match_code(const Conditions& conditions, const error_code& ec) {
    return hana::unpack(conditions, [&ec](auto const& ...x) {
        return ((ec == x) || ... || false);
    });
}

namespace impl {

namespace asio = boost::asio;
namespace hana = boost::hana;

template <code Code>
struct codes_for_condition;

template <>
struct codes_for_condition<connection_error> {
    constexpr static auto value = hana::make_tuple(
        // SQL state
        ozo::sqlstate::connection_exception,
        // Asio errors
        asio::error::broken_pipe,
        asio::error::connection_aborted,
        asio::error::connection_refused,
        asio::error::connection_reset,
        asio::error::fault,
        asio::error::host_unreachable,
        asio::error::interrupted,
        asio::error::network_reset,
        asio::error::not_connected,
        asio::error::operation_aborted,
        asio::error::shut_down,
        asio::error::timed_out,
        asio::error::try_again,
        // Asio netdb_category
        asio::error::host_not_found,
        asio::error::host_not_found_try_again,
        asio::error::no_data,
        asio::error::no_recovery,
        // System IO-error
        boost::system::errc::io_error,
        // OZO libpq-related errors
        ozo::error::pq_connection_start_failed,
        ozo::error::pq_socket_failed,
        ozo::error::pq_connection_status_bad,
        ozo::error::pq_connect_poll_failed,
        ozo::error::pg_send_query_params_failed,
        ozo::error::pg_consume_input_failed,
        ozo::error::pg_set_nonblocking_failed,
        ozo::error::pg_flush_failed
    );
};

template <>
struct codes_for_condition<database_readonly> {
    constexpr static auto value = hana::make_tuple(ozo::sqlstate::read_only_sql_transaction);
};

template <>
struct codes_for_condition<introspection_error> {
    constexpr static auto value = hana::make_tuple(
        ozo::error::bad_result_process,
        ozo::error::bad_object_size,
        ozo::error::bad_array_size,
        ozo::error::bad_array_dimension,
        ozo::error::bad_composite_size,
        ozo::error::unexpected_eof
    );
};

template <>
struct codes_for_condition<type_mismatch> {
    constexpr static auto value = hana::make_tuple(ozo::error::oid_type_mismatch);
};

template <>
struct codes_for_condition<protocol_error> {
    constexpr static auto value = hana::make_tuple(
        ozo::error::no_sql_state_found,
        ozo::error::result_status_unexpected,
        ozo::error::result_status_empty_query,
        ozo::error::result_status_bad_response,
        ozo::error::oid_request_failed
    );
};

template<code Code>
constexpr bool match_code(const error_code& ec) {
    return ozo::errc::match_code(codes_for_condition<Code>::value, ec);
}

class category final : public error_category {
public:
    const char* name() const noexcept override final { return "ozo::errc::category"; }

    std::string message(int value) const override final {
        switch (code(value)) {
            case ok :
                return "no error";
            case connection_error :
                return "communication error";
            case database_readonly:
                return "database in read-only state";
            case introspection_error:
                return "serialization/deserialization related error";
            case type_mismatch:
                return "expected type mismatch received type";
            case protocol_error:
                return "protocol-related error";
        };
        return "no message for value: " + std::to_string(value);
    }

    bool equivalent (const error_code& code, int condition) const noexcept override final {
#define OZO_ERRC_CONDITION_MATCH(name) case name: return match_code<name>(code);
        switch (condition) {
            case ok : return code == error_code{};
            OZO_ERRC_CONDITION_MATCH(connection_error);
            OZO_ERRC_CONDITION_MATCH(database_readonly);
            OZO_ERRC_CONDITION_MATCH(introspection_error);
            OZO_ERRC_CONDITION_MATCH(type_mismatch);
            OZO_ERRC_CONDITION_MATCH(protocol_error);
        }
#undef OZO_ERRC_CONDITION_MATCH
        return error_category::equivalent(code, condition);
    }
};

} // namespace impl

inline const error_category& category() noexcept {
    static impl::category instance;
    return instance;
}

} // namespace errc

} // namespace ozo
