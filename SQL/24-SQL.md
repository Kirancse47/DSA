# SQL — Q&A
> Source: `Kirancse47/DSA` -> `SQL/` — 9 files

## SQL Cheat Sheet
- **LEFT JOIN**: Returns all records from the left table and matched records from the right.
- **Self JOIN**: Joining a table to itself to compare rows within the same table.
- **GROUP BY + HAVING vs WHERE**: `WHERE` filters rows before aggregation, `HAVING` filters aggregated groups.
- **Nth Highest Value Idiom**: Use `GROUP BY` and `HAVING COUNT(DISTINCT col) = N`, or `ORDER BY ... DESC LIMIT 1 OFFSET N-1`.
- **Finding/Deleting Duplicates**: Find with `GROUP BY ... HAVING COUNT(*) > 1`. Delete with `DELETE p1 FROM t p1, t p2 WHERE p1.val = p2.val AND p1.id > p2.id`.
- **Subqueries**: Used for multi-step filtering, such as `NOT IN (SELECT...)` or finding the maximum per group before joining.

## 1. 1148. Article Views I

**Q:** Retrieve distinct author IDs who viewed their own articles, sorted by ID.

**A:** Basic filtering with DISTINCT and ORDER BY.

**Explanation:**
- Filters rows where `author_id = viewer_id` to identify authors reading their own articles.
- Uses `DISTINCT` to ensure each author ID appears only once.
- Uses `ORDER BY id` to sort the results.

```sql
select distinct(author_id) as id
from views 
where author_id = viewer_id
order by id
```

## 2. 175. Combine Two Tables

**Q:** Retrieve first name, last name, city, and state for all persons, including those without an address.

**A:** LEFT JOIN.

**Explanation:**
- Uses `LEFT JOIN` on `PersonId` to combine `Person` and `Address` tables.
- Ensures all records from the `Person` table are included, even if the matching address doesn't exist (returns `NULL` for missing data).

```sql
select FirstName, LastName, City, State from Person p left join Address a on p.PersonId = a.PersonId;
```

## 3. 176. Second Highest Salary

**Q:** Find the second highest salary from the Employee table.

**A:** Self join with HAVING or LIMIT / OFFSET.

**Explanation:**
- The first approach uses a self join and counts distinct higher or equal salaries, filtering with `HAVING COUNT(DISTINCT e2.salary) = 2`.
- The second approach uses `ORDER BY salary DESC` and skips the top salary using `OFFSET 1`, returning the next row with `LIMIT 1`.

```sql
select (select e1.salary from employee e1 join employee e2 where e1.salary<=e2.salary group by e1.salary having count(distinct e2.salary)=2) as SecondHighestSalary;
  
  # or 
  
select (select distinct salary from employee order by salary desc limit 1 offset 1) as SecondHighestSalary;
```

## 4. 177. Nth Highest Salary

**Q:** Find the Nth highest salary from the Employee table.

**A:** Self join with GROUP BY and HAVING.

**Explanation:**
- Joins the `employee` table to itself on `e1.salary <= e2.salary`.
- Groups by `e1.salary` and counts distinct salaries that are greater than or equal to the current one.
- Returns the salary where exactly `N` distinct salaries are greater than or equal to it.

```sql
select e1.salary from employee e1 join employee e2 where e1.salary<=e2.salary group by e1.salary having count(distinct e2.salary)=N
```

## 5. 181. Employees Earning More Than Their Managers

**Q:** Find all employees who earn more than their direct managers.

**A:** Self join.

**Explanation:**
- Joins the `Employee` table with itself, matching an employee (`e1`) to their manager (`e2`).
- The join condition relies on `e1.managerId = e2.id`.
- Filters rows where the employee's salary is strictly greater than the manager's salary.

```sql
# Using Where Clause...
select e1.name as Employee from Employee e1,Employee e2 where e1.managerId=e2.id and e1.salary>e2.salary;

# Using Join Clause...
select e1.name as Employee from Employee e1 join Employee e2 where e1.managerId=e2.id and e1.salary>e2.salary;
```

## 6. 182. Duplicate Emails

**Q:** Find all duplicate emails in the Person table.

**A:** GROUP BY with HAVING.

**Explanation:**
- Groups records by the `email` column.
- Uses `HAVING COUNT(*) > 1` to filter and return only those emails that appear multiple times.

```sql
select Email from Person group by email having count(*)>1;
```

## 7. 183. Customers Who Never Order

**Q:** Find all customers who have never placed an order.

**A:** NOT IN subquery.

**Explanation:**
- Uses a subquery to select all `customerId`s present in the `orders` table.
- The main query selects customer names where their `id` does not appear in that subquery result list.

```sql
select name as Customers from Customers where id not in (select customerId from orders);
```

## 8. 184. Department Highest Salary

**Q:** Find employees who have the highest salary in each of the departments.

**A:** JOIN with a grouped subquery.

**Explanation:**
- A subquery calculates the `MAX(salary)` grouped by `departmentId`.
- This result is joined with the `department` table to get department names alongside max salaries.
- The outermost query joins the `employee` table against this derived table to fetch the names of employees matching both the department and the max salary.

```sql
select r.name as Department,e.name as Employee,r.salary from 
employee e join (select d1.id,d1.name, e2.salary from department d1 join (select departmentId,max(salary) salary from employee group by departmentId) e2
on d1.id=e2.departmentId) r on e.salary=r.salary and e.departmentId=r.id;
```

## 9. 196. Delete Duplicate Emails

**Q:** Delete duplicate emails, keeping only the record with the smallest ID.

**A:** DELETE with a self join or a NOT IN subquery.

**Explanation:**
- The first approach finds the minimum `id` per email and deletes all IDs not in that set.
- The second approach uses a self-join (`p1`, `p2`) on email, deleting the record from `p1` whenever it has the same email as `p2` but a strictly greater `id`.

```sql
# Using where and not in..
delete from Person where id not in (select * from (select min(id) from Person group by email) as p);

# Using where...
delete p1 from person p1,person p2 where p1.email=p2.email and p1.id>p2.id;
```
