# SalesAnalyzer

## Introduction
SalesAnalyzer is a system designed to analyze product sales across various provinces and cities. This project aims to provide insights into sales data by aggregating information from multiple sources and performing detailed analyses.

## General Description
The primary objective of SalesAnalyzer is to analyze sales data for different products in various cities and provinces. The input to the system will be provided through a C program, which aggregates sales information from multiple stores within a city. Each store contributes its sales data to the system, which is then stored in a PostgreSQL database for further analysis.

### Key Components
1. **C Program**: Aggregates sales data and stores it in a PostgreSQL database.
2. **Python Program**: Reads aggregated data from the database and performs various analyses.
3. **Data Input**: Sales data is processed from files formatted specifically for this project.

## Features
- **Data Aggregation**: The C program reads input files, aggregates sales data by city and store, and organizes it into structured tables in the database.
- **Flexible Analysis**: The Python program runs analyses on the aggregated data, providing insights into sales trends and product performance.
- **Automated Data Processing**: The system is designed to handle incoming sales data efficiently, ensuring timely updates to the database.

## Installation and Usage

### Prerequisites
- C Compiler (e.g., GCC)
- Python 3.x
- PostgreSQL Database

### Installation Steps
1. **Clone the Repository**:
     ```bash
     git clone https://github.com/yourusername/SalesAnalyzer.git
     cd SalesAnalyzer
     ```
2. **Set Up Database**:
  - Create a PostgreSQL database named ``fpdb``.
  - Run the necessary SQL scripts to create required tables:
    ```sql
    CREATE TABLE data_stores_fp (...);
    CREATE TABLE fp_city_aggregation (...);
    CREATE TABLE aggregation_store_fp (...);
    ```
3. **Download Input Data**:
  - Use the provided Bash script to download sales data:
    ```bash
    bash download_data.sh
    ```
4. **Compile and Run C Program**:
    ```bash
    gcc -o sales_aggregator sales_aggregator.c
    ./sales_aggregator
    ```
5. **Run Python Analysis**:
  - Schedule the Python program to run every half hour using cron or manually execute it:
    ```bash
    python analyze_sales.py
    ```
## Data Input Format
The input files should follow this format:
```text
time, province, city, market_id, product_id, price, quantity, has_sold
```
Each line represents a record of sales data.
## C Program Details
The C program performs the following tasks:
  1. Reads input files from ``/Sales_Analyzer/tmp/``.
  2. Aggregates data into three tables:
     - ``data_stores_fp``: Raw sales data.
     - ``fp_city_aggregation``: Aggregated city-level sales data.
     - ``aggregation_store_fp``: Store-level sales aggregates.
### Logging
Implement logging to track various stages of execution (e.g., database connection status). Instructions for defining a service in Linux and using logging libraries will be provided in the project documentation.
## Python Program Details
The Python program analyzes aggregated data every half hour. An example analysis includes determining the most popular product in each province based on quantity sold and price.
## Conclusion
SalesAnalyzer provides valuable insights into sales trends across different regions by effectively aggregating and analyzing data. Each component works together seamlessly to facilitate detailed examination of product performance. For any questions or contributions, please feel free to open an issue or submit a pull request!
