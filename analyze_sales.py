import psycopg2
#list of checked provinces
provinces=[]
#checks if a province is seen before
#which the previous has sold more
#if not then adds it to list
def checking(province):
    if province in provinces:
        return False
    else:
        provinces.append(province)
        return True
try:
    #connecting to data base
    connection =psycopg2.connect(dbname="fpdb",user="farnood")
    cursor = connection.cursor()
    ##1)best product of each province
    string="select province,product_id,sum(fp_stores_data.has_sold),AVG(price)  From fp_stores_data group by province,product_id order by sum(fp_stores_data.has_sold) DESC;"
    cursor.execute(string)
    record = cursor.fetchall()
    #open file
    productProvince=open("Best_Product_of_Each_Province.csv","w")
    productProvince.write("province\tproduct_id\thas_sold\taverage_price\n")
    for i in record:
        if (checking(i[0])):
            productProvince.write("%s\t%s\t%s\t%s\n"%(i[0],i[1],i[2],i[3]))
    #close file
    productProvince.close()
    ##2)best product of country
    string="SELECT product_id,SUM(fp_stores_data.has_sold),AVG(price) FROM fp_stores_data GROUP BY product_id ORDER BY SUM(fp_stores_data.has_sold) DESC LIMIT 10;"
    cursor.execute(string)
    record = cursor.fetchall()
    #open file
    bestProductOFCountry=open("Best_Product_of_Country.csv","w")
    bestProductOFCountry.write("product_id\thas_sold_total\taverage_price\n")
    for i in record:
        bestProductOFCountry.write("%s\t%s\t%s\n"%(i[0],i[1],i[2]))
    #close file
    bestProductOFCountry.close()
    ##3)most profitable store
    string="SELECT * FROM fp_store_aggregation ORDER BY earned DESC LIMIT 10;"
    cursor.execute(string)
    record=cursor.fetchall()
    #open file
    profitableStore=open("Most_Profitable_Store_of_Country.csv","w")
    profitableStore.write("market_id\tearned\n")
    for i in record:
        profitableStore.write("%s\t%s\n"%(i[0],i[2]))
    #close file
    profitableStore.close()
    ##4)cities with most goods in
    string="SELECT city,SUM(quantity) FROM fp_city_aggregation GROUP BY city ORDER BY SUM(quantity) DESC LIMIT 10;"
    cursor.execute(string)
    record=cursor.fetchall()
    #open file
    cityGoods=open("Cities_With_Most_Goods.csv","w")
    cityGoods.write("city\tgoods\n")
    for i in record:
        cityGoods.write("%s\t%s\n"%(i[0],i[1]))
    #close file
    cityGoods.close()
    ##5)cities with highest demands
    string="SELECT city FROM fp_city_aggregation GROUP BY city ORDER BY SUM(has_sold-quantity) DESC LIMIT 10;"
    cursor.execute(string)
    record=cursor.fetchall()
    #open file
    cityDemand=open("Cities_with_Highest_Demands.csv","w")
    cityDemand.write("city\n")
    for i in record:
        cityDemand.write("%s\n"%i)
    #close file
    cityDemand.close()
    ##6)cities with lowest demands
    string="SELECT city FROM fp_city_aggregation GROUP BY city ORDER BY SUM(has_sold-quantity) ASC LIMIT 10;"
    cursor.execute(string)
    record=cursor.fetchall()
    #open file
    cityLowDemand=open("Cities_with_Lowest_Demands.csv","w")
    cityLowDemand.write("city\n")
    for i in record:
        cityLowDemand.write("%s\n"%i)
    #close file
    cityLowDemand.close()
#handle exception of connection
except (Exception, psycopg2.Error) as error :
    print ("Error while connecting to PostgreSQL", error)
finally:
    #closing database connection.
        if(connection):
            cursor.close()
            connection.close()
            print("Finished")
