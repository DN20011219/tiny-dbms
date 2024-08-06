# 1、写在前面
如何实现一个DBMS呢？一个数据库管理系统是如何从头到尾设计的？
本项目将从简单到复杂，实现一个最基础的关系型数据库管理系统。在此基础上，还准备为其拓展适配向量数据库的部分功能。
# 2、版本0.0.1
本版本是一个基础的数据库管理系统。
系统通过使用启动时创建的命令行进行控制，不存在并发问题、不考虑数据的安全性。通过最简单方法实现一个数据库管理系统。以此理解关系型数据库的几大基本模块。
# 3、系统设计
## 3.1、SQL解析
如何高效地执行一条SQL？这是一个开放性问题，根据系统特性、数据特性，我们可以选择使用任何方法。但是当前的DBMS几乎全部无一例外地选择了将解析与执行分为两个模块，独立进行设计的思路。
SQL解析可以理解为一种编译手段，只不过编译的结果是一棵给下层执行模块使用的语法树，而错误的SQL（语法错误，语意错误）都会无法被编译得到正确结果。
阿里巴巴的MiniOB教学文档中提到了一条SQL的具体执行过程：
![SQL-执行](./picture/alibaba_sql_excution.png)
其中第一步，一棵AST的例子如下：
![AST-摘自美团技术文档](./picture/ast_example.png)
可以理解AST是一棵不受下层实现影响的语法树，任何SQL经由语法分析得到的AST都是相同的（实际上可能由于树的约定表述逻辑不同，得到的AST的结构并不完全相同，但是其中蕴含的语意必定是相同的）。AST是下层语意解析的基础，语意解析将AST解析成数据库中具体的对象（见上图）。
由于支持全部的SQL的工作量过于巨大，如果全部手写很难在短时间内完成：
> Writing and maintaining our own SQL parser is a bad idea. SQL is complex, even for simple things like SELECT. And don't get me started on Common Table Expressions, sub-queries and other fun features

因此我选取了一部分核心SQL，提供解析执行能力。此版本具体支持的SQL特性见下图：
![支持的SQL](./picture/support_sql_type_now.png)
未来如果有精力，我还将引入三方库（如：libpg_query，postgres_parser，lex&yacc）实现SQL解析，从而支持更多的SQL特性：
![后续计划支持的SQL](./picture/support_sql_type_future.png)

参考资料：
> 1、美团：什么是SQL解析：
> https://tech.meituan.com/2018/05/20/sql-parser-used-in-mtdp.html
> 2、SQL定义：
> https://www.w3schools.com/sql/sql_where.asp
> 3、Introducing pg_query: Parse PostgreSQL queries in Ruby
> https://pganalyze.com/blog/parse-postgresql-queries-in-ruby
> 4、PostgreSQL SQL解析引擎
> https://github.com/pganalyze/libpg_query
> 5、bustub 卡耐基梅隆 CMU 15445 项目
> https://github.com/cmu-db/bustub
> 6、阿里巴巴miniOB手册
> https://obcommunity-private-oss.oceanbase.com/prod/blog/2023-09/%E4%BB%8E0%E5%88%B01%20OceanBase%E5%8E%9F%E7%94%9F%E5%88%86%E5%B8%83%E5%BC%8F%E6%95%B0%E6%8D%AE%E5%BA%93%E5%86%85%E6%A0%B8%E5%AE%9E%E6%88%98%E5%9F%BA%E7%A1%80%E7%89%88.pdf

### 3.1.1 分词器（词法分析）
分词器的作用是将一条SQL语句分解为一个个的单词（token），也就是所谓的正则匹配。本系统支持的SQL暂时较为简单，支持解析的字段很少，因此可以根据本版本支持的SQL，设计一个简单的词法分析器。
#### 1.初步设计
1、根据预留关键词（如SELECT、UPDATE、左右括号、分号、逗号、空格等）进行匹配，将匹配到的关键词进行归类并设置类型，若所有关键词都无法进行匹配，则转向非关键词匹配
2、根据非预留关键词类型进行匹配，此处可能涉及到各种系统需要支持的数据的具体过滤方法，比如：表、列、数据库名称只允许出现英文字母和_，且两端不会被双引号包含；string类型的数据需要支持任何字符任何长度，但是两端会被双引号包含；日期类型的数据只支持"2024-08-01"这种模式的字符串。如果匹配到了，那么还需要将其设置为对应类型的数据。
3、从前到后将所有匹配到的数据类型和数据输出为一个列表，以供后续语法分析器进行分析。
如果匹配过程中出现了任意无法匹配的情况，那么都将直接返回错误。
#### 2.分词器功能举例
> 预留关键词：SELECT 空格 FROM 逗号 分号
> 非预留关键词: string：由任意长度任意类型字符构成的串，但是两端要被双引号包含起来。 id: 表示数据库名、表名、列名、索引名的数据，可以是任意非预留关键词，但是两端不会被括号括起来，只允许出现英文字母和_。

输入数据： 
> 数据1（合法）: SELECT test FROM table1_tt_ ;
> 数据2（非合法，又一个单独的双引号）: SELECT Table SELECT FROM table " ;

解析结果
> 数据1结果：|select: SELECT | id：test ｜from: FROM| id: table1_tt_| semicolon: ;|
> 数据2结果：|select: SELECT | id：Table | select: SELECT | from: FROM | id: table | 无法被解析的字符串，返回失败！
#### 3.总结
通过上述词法分析流程，一条完整的SQL应该被分成了若干个token，并且去除了无效信息，如空格。
#### 4.实现
上述分词器实现位于sql/parser/separater.h
### 3.1.2 语法分析
经过第一步的词法分析，一条SQL已经被解析成了一长串全部符合规定的token链表。而语法分析要做的就是判断这串token是否符合系统支持的sql格式。比如：判断一条SQL是否符合语法标准（SELECT后面必须跟着id，FROM后面必须跟着id，SELECT后面必须有FROM节点且它们之间只能是若干个被逗号分隔的id），SQL中提到的关键数据是否都存在（数据库，表，字段名），输入的数据是否合法（字段名不应该存在大写字母， 日期数据必须是“2024-01-01”这类格式）
#### 1.初步设计
语法分析首先要确定系统需要支持的语法都有哪些，以下是一些简单的语法：

> id: 不被双引号扩起来的，只有英文字母和下划线的字符串
其他id: , id
属性（可以被匹配为多种数据类型）: INT STRING FLOAT
比较符（可以被匹配为多种数据类型）: > < = >= <= <> != 
关系符（可以被匹配为多种数据类型）: AND OR
其他比较: , 关系符 id 比较符属性
其他属性: , 属性

> 创建数据库：CREATE DATABASE id;
删除数据库: DROP DATABASE id;
创建表: CREATE TABLE id ( id 其他id );
删除表: DROP TABLE id;
插入语句：INSERT INTO id (id 其他id) VALUES (属性 其他属性) ;
查询语句：SELECT id 其他id FROM id ;
查询语句(带条件)：SELECT id 其他id FROM id WHERE id 比较符 属性 其他比较;

#### 2.分词器功能举例
输入SQL:
>  CREATE TABLE test_table ( id int, id2 vchar);

通过分词器，将得到一个token列表，如下：
> KEY_WORD: CREATE | KEY_WORD:TABLE | ID: test_table | op: ( | ID: id | KEY_WORD:int | ID: id2 | KEY_WORD:vchar | op: ) |

创建表SQL语法如上节，通过匹配，可以识别出几个创建表所需的数据：
> 表名：test_table
列名列表：id, id2
列类型列表：int, vchar

上述实现可阅读include/sql/sql_pattern和include/sql/ast，两个文件实现了sql语法定义，sql语意解析，构建AST的过程。
#### 3.总结
词法解析最终得到的是一棵可以用于规划执行计划的语法树。不同类型的SQL得到的语法树结构是大不相同的，因此执行器需要针对每种SQL都设计对应的执行计划。
### 3.1.3 执行器
拿到了语法树，最后一步就是解析执行。
执行器将AST转换为系统的执行计划，通过顺序/递归等方式构建系统中各个函数的执行流程。

## 3.2、存储引擎