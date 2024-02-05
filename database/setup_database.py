import sqlite3
import db_pb2

with sqlite3.connect("database.db") as conn:
    cursor = conn.cursor()

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS static_data (
            identifier TEXT PRIMARY KEY,
            data BLOB NOT NULL
        )         
    ''')

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS dyn_data (
            problem_id INTEGER NOT NULL,
            task_id INTEGER NOT NULL,
            data BLOB NOT NULL
        )
    ''')

    l_list = db_pb2.List()
    l_list.values.extend([1, 2, 3, 4, 5])
    l_serialized = db_pb2.Field()
    l_serialized.list.CopyFrom(l_list)

    k_list = db_pb2.List()
    k_list.values.extend([-6, -7, -11, -9, -10])
    k_serialized = db_pb2.Field()
    l_serialized.list.CopyFrom(k_list)

    x_scalar = db_pb2.Scalar()
    x_scalar.value = 420
    x_serialized = db_pb2.Field()
    x_serialized.scalar.CopyFrom(x_scalar)

    query = "INSERT INTO static_data (identifier, data) VALUES (?, ?)"

    cursor.execute(query, ("L", l_serialized.SerializeToString()))
    cursor.execute(query, ("K", k_serialized.SerializeToString()))
    cursor.execute(query, ("x", x_serialized.SerializeToString()))

    conn.commit()
