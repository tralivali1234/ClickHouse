#pragma once

#include <DB/DataStreams/IProfilingBlockInputStream.h>
#include <DB/Columns/ColumnConst.h>
#include <DB/Core/ColumnWithTypeAndName.h>

namespace DB
{

/** Добавляет в блок материализованный const column с заданным значением.
  */
template <typename ColumnType>
class AddingConstColumnBlockInputStream : public IProfilingBlockInputStream
{
public:
	AddingConstColumnBlockInputStream(
		BlockInputStreamPtr input_,
		DataTypePtr data_type_,
		ColumnType value_,
		String column_name_)
		: data_type(data_type_), value(value_), column_name(column_name_)
	{
		children.push_back(input_);
	}

	String getName() const override { return "AddingConstColumn"; }

	String getID() const override
	{
		std::stringstream res;
		res << "AddingConstColumn(" << children.back()->getID() << ")";
		return res.str();
	}

protected:
	Block readImpl() override
	{
		Block res = children.back()->read();
		if (!res)
			return res;
		ColumnPtr column_ptr = ColumnConst<ColumnType>(res.rows(), value, data_type).convertToFullColumn();
		res.insert({column_ptr, data_type, column_name});
		return res;
	}

private:
	DataTypePtr data_type;
	ColumnType value;
	String column_name;
};

}
