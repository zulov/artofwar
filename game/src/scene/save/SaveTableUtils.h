#pragma once

#include "SaveTable.h"
#include "database/db_insert_utils.h"

template <typename Col, typename CreateTable, typename WriteRows>
bool saveTable(sqlite3* database, CreateTable createTable, WriteRows writeRows) {
	if (!createTable(SaveTable<Col>::name, SaveTable<Col>::schema)) {
		return false;
	}

	const auto sql = make_insert_sql(SaveTable<Col>::name, saveColumns<Col>());
	return executeBatch(database, sql.c_str(), writeRows);
}
