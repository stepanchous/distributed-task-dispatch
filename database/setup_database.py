import sqlite3
import db_pb2
import json

with sqlite3.connect("database.db") as conn:
    cursor = conn.cursor()

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS static_data (
            identifier TEXT PRIMARY KEY,
            data BLOB NOT NULL
        );        
    ''')

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS dynamic_data (
            problem_id INTEGER NOT NULL,
            task_id INTEGER NOT NULL,
            data BLOB NOT NULL,
            PRIMARY KEY (problem_id, task_id)
        );
    ''')

    query = "INSERT INTO static_data (identifier, data) VALUES (?, ?)"

    with open("data.json") as f:
        var_to_value = json.load(f)

        for var, value in var_to_value.items():
            field = db_pb2.Field()
            if var.islower():
                value_pb = db_pb2.Scalar()
                value_pb.value = value
                field.scalar.CopyFrom(value_pb)
            else:
                list_pb = db_pb2.List()
                for el in value:
                    list_pb.values.append(el)
                field.list.CopyFrom(list_pb)

            cursor.execute(query, (var, field.SerializeToString()))

    conn.commit()
