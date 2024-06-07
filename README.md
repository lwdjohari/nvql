# NvQL
Unified Data Layer Library for postgres, oracle, mysql &amp; sqlite database for C++ 14 &amp; 17.<br/>

```NvQL``` is being developed primarily for NvServ, a rapid server framework for modern c++ data layer.<br/>
Serving for easy to use low overhead unified abstraction data layer

<u>Database supported</u>
- Postgres : WIP
- Oracle : WIP
- Mysql : Plan
- Sqlite: Plan
  
> [!WARNING]
> Currently compatibility with C++14 is not yet throughly tested.<br/>
> Status : WIP, Experimental & Unstable.

## Design Concept

Inspired by userver microservice framework,<br/>
we're decided to take it further into unified data layer.<br/>

In-short, no matter database you use, all the API design are same acrross different DB.<br/>
High quality, High productivity whatever database you choose.

```cxx

// Using the storage-config.yaml
StorageServerPtr server = PgServer(100,25);

// what if we use oracle as our DB
// StorageServerPtr server = OracleServer(100,25);

// Create default transaction (read/write)
auto tx = server->Begin();

int32_t status_param = 1;
auto result =
    tx->Execute("select * from employee where status = $1", status_param);

if (!result->IsEmpty()) {
  for (auto row : *result) {
    auto emp_id = row->As<int32_t>("emp_id");
    auto emp_name = row->As<std::string>("emp_name");
    auto dept_id = row->As<std::string>("dept_id");
    auto sect_name = row->As<std::optional<std::string>>("sect_name");
    auto status = row->As<int32_t>("status");
    auto dob = row->As<nvm::dates::DateTime>("dob");

    std::cout << "[" << emp_id << "] " << emp_name << " (" << dept_id << ":"
              << (sect_name.has_value() ? sect_name.value() : "") << ", "
              << dob << ", " << status
              << ")" <<
              std::endl;
  }
}
// No need for manual tx.Rollback() or tx.Release
// When TransactionPtr out-of-scope
// automatically checking what need to do.
```
## Implementations

NvQL by default make this as the first-class citizen and being implement under-the-hood
- Cluster Connection & fallback mechanism
- Connection Pool & connection idle wake up
- Transaction Mode (options to choose different mode of transactions)
- Binary Transport Protocol if supported & fallback mechanism
- Prepared Statement (nvql manages this automatically and all execution)
- Parameterized parameter (auto sanitazion)
  
nvql manages the acquire and returning connection and <br/>
wiring all the necessity of transaction <br/>
even the prepared statement execution.

nvql still give developer access to underlying library for each database server.
This give flexibility for developer  as the last resort and give big relief when we facing edge cases.

nvql from-day-one and by design is not suppose to support implementation
- transaction-less query
- sql query string execution (without prepared statement)
- multi-level/nested transactions


## Motivation
During years of handling development that are involved database, <br/>
we are facing same conceptual and fundamentals but different implementation for different database server.
One thing faster in one area and one thing have higher effort on certain area, repeat.
Thats the reason why NvServ Server Framework & nvql were born.

## Main Engine under-the-hood

Under the hood ```nvql``` utilizes abstracting proven library in industry
- postgres: pqxx v7
- oracle: occi
- mysql: coming soon
- sqlite: coming soon 

## Contributions

Currently we are still on-going roadmap design and architectural design that might be lead to complete rewrite or complete breaking changes.
We might accept contributors when everything above have better & crytal-clears roadmap.

## License

Copyright [2024] [Linggawasistha Djohari]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
