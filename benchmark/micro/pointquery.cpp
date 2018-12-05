#include "benchmark_runner.hpp"
#include "duckdb_benchmark.hpp"

#include <random>

using namespace duckdb;
using namespace std;

#define POINT_QUERY_ROW_COUNT 100000000
#define POINT_QUERY_ENTRY 50000

DUCKDB_BENCHMARK(PointQueryWithoutIndex, "[micro]")
virtual void Load(DuckDBBenchmarkState *state) {
	state->conn.Query("CREATE TABLE integers(i INTEGER, j INTEGER);");
	auto appender = state->conn.GetAppender("integers");
	// insert the elements into the database
	for (size_t i = 0; i < POINT_QUERY_ROW_COUNT; i++) {
		appender->begin_append_row();
		appender->append_int(i);
		appender->append_int(i + 2);
		appender->end_append_row();
	}
	state->conn.DestroyAppender();
}

virtual string GetQuery() {
	return "SELECT j FROM integers WHERE i=" + to_string(POINT_QUERY_ENTRY);
}

virtual string VerifyResult(DuckDBResult *result) {
	if (!result->GetSuccess()) {
		return result->GetErrorMessage();
	}
	if (result->size() != 1) {
		return "Incorrect amount of rows in result";
	}
	if (result->column_count() != 1) {
		return "Incorrect amount of columns";
	}
	if (result->GetValue<int>(0, 0) != POINT_QUERY_ENTRY + 2) {
		return "Incorrect result returned, expected " + to_string(POINT_QUERY_ENTRY + 2);
	}
	return string();
}

virtual string BenchmarkInfo() {
	return StringUtil::Format("Runs the following query: \"" + GetQuery() + "\" without an index");
}
FINISH_BENCHMARK(PointQueryWithoutIndex)

DUCKDB_BENCHMARK(PointQueryWithIndex, "[micro]")
virtual void Load(DuckDBBenchmarkState *state) {
	state->conn.Query("CREATE TABLE integers(i INTEGER, j INTEGER);");
	auto appender = state->conn.GetAppender("integers");
	// insert the elements into the database
	for (size_t i = 0; i < POINT_QUERY_ROW_COUNT; i++) {
		appender->begin_append_row();
		appender->append_int(i);
		appender->append_int(i + 2);
		appender->end_append_row();
	}
	state->conn.DestroyAppender();
	state->conn.Query("CREATE INDEX i_index ON integers(i)");
}

virtual string GetQuery() {
	return "SELECT j FROM integers WHERE i=" + to_string(POINT_QUERY_ENTRY);
}

virtual string VerifyResult(DuckDBResult *result) {
	if (!result->GetSuccess()) {
		return result->GetErrorMessage();
	}
	if (result->size() != 1) {
		return "Incorrect amount of rows in result";
	}
	if (result->column_count() != 1) {
		return "Incorrect amount of columns";
	}
	if (result->GetValue<int>(0, 0) != POINT_QUERY_ENTRY + 2) {
		return "Incorrect result returned, expected " + to_string(POINT_QUERY_ENTRY + 2);
	}
	return string();
}

virtual string BenchmarkInfo() {
	return StringUtil::Format("Runs the following query: \"" + GetQuery() + "\" with an index");
}
FINISH_BENCHMARK(PointQueryWithIndex)