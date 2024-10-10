#include<stdio.h>
#include<string.h>
#include<string.h>
#include<dirent.h>
#include<sys/types.h>
#include<stdlib.h>
#include<postgresql/libpq-fe.h>
#include<arpa/inet.h>
int main()
{
	int limit=0,first=1,citySold=0,cityQuantity=0;
	long long cityAVG=0,storeEarned=0;
	char tmp[100],line[100], separate[8][50],checkCityTime[2][50]={0};
	//directory of downloaded folder
	char* dir="/tmp/Sales_Analyzer/";
	struct dirent* in_file;
	FILE *myFile;
	//make FD a directory pointer
	DIR* FD=opendir(dir);
	if(FD==NULL)
	{
		printf("couldn't find folder!\n");
		return 0;
	}
	//connect to data base
	PGconn *conn=PQconnectdb("dbname=fpdb");
	//check connection
	if (PQstatus(conn)==CONNECTION_BAD)
	{
		printf("connection to database failed: %s\n"
		,PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}
	//read each file in folder directory to open
	while (in_file=readdir(FD))
	{
		//avoid . and .. files
		if (!strcmp (in_file->d_name, "."))
            		continue;
        	if (!strcmp (in_file->d_name, ".."))    
            		continue;
		//make tmp directory of file
		strcpy(tmp,dir);
		strcat(tmp,in_file->d_name);
		//open file
		myFile=fopen(tmp,"r");
        	if (myFile==NULL) 
		{
			printf("NOFILE %s\n",in_file->d_name);
			continue;
		}
		printf("%s:\n",in_file->d_name);
		//separate each line and puts in array
		while (1)		
		{
			limit=0;
			//get each line
			fgets(line,99,myFile);
			if (feof(myFile)==1)break;
			//replace \n with \0
			for(int i=0;line[i]!='\0';i++)
				if(line[i]=='\n')
					line[i]='\0';
			//first token
			char* token=strtok(line,",");
			//maximum of 8 preventing crash
			for(int i=0;token!=NULL && i<8;i++)
			{
				if (token[0]==' ')
					strcpy(separate[i],token+1);
				else
					strcpy(separate[i],token);
				token=strtok(NULL,",");
				limit=i;
			}
			limit+=1;
			//empty uninitialized tokens
			for (;limit<8;limit++)
			{
				strcpy(separate[limit],"");
			}
			//*********************
			//FILL fp_stores_data *
			//*********************
			int lengths[8]={50,50,50,50,50,50,50,50},binary[8]={0};
			//make const char for PGresult
			const char* values[8]={separate[0],separate[1],
			separate[2],separate[3],separate[4],
			separate[5],separate[6],separate[7]};
			//command
			PGresult *res =PQexecParams(conn 
			,"INSERT INTO fp_stores_data VALUES($1::varchar,$2::varchar,$3::varchar,$4::varchar,$5::varchar,$6::int4,$7::int4,$8::int4)"
			,8,NULL,values,lengths,binary,0);
			//check command
			if (PQresultStatus(res) != PGRES_COMMAND_OK) 
			{
				printf("fp_stores_data crashed: %s\n",PQerrorMessage(conn));  
			}
			//**************************
			//FILL fp_city_aggregation *
			//**************************
			//if change in city or time happend and not the first line
			if ((strcmp(checkCityTime[1],separate[2])||strcmp(checkCityTime[0],separate[0]))&&!first)
			{
				int lengths[5]={50,50,50,50,50},binary[5]={0};
				char tmp1[50],tmp2[50],tmp3[50];
				//make price average
				cityAVG/=citySold;
				//int to string
				sprintf(tmp1,"%d",cityQuantity);sprintf(tmp2,"%d",citySold);
				sprintf(tmp3,"%lld",cityAVG);
				//make const char for PGresult
				const char* values[5]={checkCityTime[1],checkCityTime[0],
				tmp1,tmp2,tmp3};
				//command
				PGresult *res =PQexecParams(conn 
				,"INSERT INTO fp_city_aggregation VALUES($1::varchar,$2::varchar,$3::int4,$4::int4,$5::int4)"
				,5,NULL,values,lengths,binary,0);
				//check command
				if (PQresultStatus(res) != PGRES_COMMAND_OK) 
				{
					printf("fp_city_aggregation crashed: %s\n",PQerrorMessage(conn));
				}
				//change city or time
				strcpy(checkCityTime[1],separate[2]);
				strcpy(checkCityTime[0],separate[0]);
				//first initialization
				citySold=atoi(separate[7]);
				cityQuantity=atoi(separate[6]);
				cityAVG=(atoi(separate[5])*atoi(separate[7]));
			}
			else
			{
				//if first line
				if(first)
				{
					first=0;
					strcpy(checkCityTime[1],separate[2]);
					strcpy(checkCityTime[0],separate[0]);
				}
				//atoi: string to int
				citySold+=atoi(separate[7]);
				cityQuantity+=atoi(separate[6]);
				cityAVG+=(atoi(separate[5])*atoi(separate[7]));
			}
			//***************************
			//FILL fp_store_aggregation *
			//***************************
			int length[3]={50,50},binary2[2]={0};
			//temp=has_sold * price
			char temp[50];sprintf(temp,"%d",(atoi(separate[7])*atoi(separate[5])));
			//two values for two updates
			const char* value1[2]={temp,separate[3]};
			const char* value2[2]={separate[7],separate[3]};
			//two commands
			PGresult *res1 =PQexecParams(conn 
			,"UPDATE fp_store_aggregation SET earned = fp_store_aggregation.earned+$1 where market_id=$2"
			,2,NULL,value1,length,binary2,0);
			PGresult *res2 =PQexecParams(conn 
			,"UPDATE fp_store_aggregation SET has_sold = fp_store_aggregation.has_sold+$1 where market_id=$2"
			,2,NULL,value2,length,binary2,0);
			//check commands
			if (PQresultStatus(res1) != PGRES_COMMAND_OK) 
			{
				printf("fp_store_aggregation1 crashed: %s\n",PQerrorMessage(conn));
			}
			if (PQresultStatus(res2) != PGRES_COMMAND_OK) 
			{
				printf("fp_store_aggregation2 crashed: %s\n",PQerrorMessage(conn));
			}
			//if market_id is not in table to update
			//we add a new row
			if (!strcmp(PQcmdStatus(res1),"UPDATE 0"))
			{
				int length3[3]={50,50,50},binary3[3]={0};
				const char* valueForThis[3]={separate[3],separate[7],temp};
				//command
				PGresult *res3 =PQexecParams(conn 
				,"INSERT INTO fp_store_aggregation VALUES($1::varchar,$2::int4,$3::int8)"
				,3,NULL,valueForThis,length3,binary3,0);
				//check command
				if (PQresultStatus(res3) != PGRES_COMMAND_OK) 
				{
					printf("fp_store_aggregation3 crashed: %s\n",PQerrorMessage(conn));
				}
			}	
		}
		//end of separation
		fclose(myFile);
		//delete file after reading
		remove(tmp);
		printf("collected and removed\n\n");
	}
	//end of connection
	PQfinish(conn);
	//end of reading all files in directory
	printf("FINISHED!\n");
}
