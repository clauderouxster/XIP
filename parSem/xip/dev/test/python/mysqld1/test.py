import MySQLdb

#db=MySQLdb.connect(host="etretat",port=3306, user="root",passwd="root",db="goldminer")
db=MySQLdb.connect(host="etretat",port=3306, user="root",passwd="root",db="goldminer" )

db.query("""SELECT * FROM conceptt """)

