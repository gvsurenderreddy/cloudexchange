#include "KeystoneInfo.hpp"

CKeystoneInfo::CKeystoneInfo()
{

}

CKeystoneInfo::~CKeystoneInfo()
{

}

int CKeystoneInfo::Init(DbConnect* pcDbConnect) 
{
	m_pcDbConnect_ = pcDbConnect;
	return ITF_OK;
}

char *CKeystoneInfo::ConvertIDToUUID(char *a_strID)
{
	static char	 strBuff[UUID_LEN];	
	char		*p_from = a_strID;
	char		*p_to = strBuff;

	memset(strBuff, 0x00, sizeof(strBuff));

	strncpy(p_to, p_from, UUID_TIME_LOW);
	p_from += UUID_TIME_LOW;
	p_to += UUID_TIME_LOW;
	*(p_to++) = '-';
	
	strncpy(p_to, p_from, UUID_TIME_MID);
	p_from += UUID_TIME_MID;
	p_to += UUID_TIME_MID;
	*(p_to++) = '-';
	
	strncpy(p_to, p_from, UUID_TIME_HIGH_VERSION);
	p_from += UUID_TIME_HIGH_VERSION;
	p_to += UUID_TIME_HIGH_VERSION;
	*(p_to++) = '-';

	strncpy(p_to, p_from, UUID_CLOCK_SEQ);
	p_from += UUID_CLOCK_SEQ;
	p_to += UUID_CLOCK_SEQ;
	*(p_to++) = '-';

	strncpy(p_to, p_from, UUID_TIME_NODE);

	return strBuff;
}

char *CKeystoneInfo::ConvertUUIDToVTNKey(char *a_uuid)
{
	static char	 strBuff[UUID_LEN];
	char		*p_from = a_uuid;
	char		*p_to = strBuff;
	char		 tempStr[KEYSTONE_ID_LEN];
	char		*itemList[10];
	int			 itemCount=0;

	memset(strBuff,  0x00, sizeof(strBuff));
	memset(tempStr,  0x00, sizeof(tempStr));
	memset(itemList, 0x00, sizeof(itemList));

	itemList[itemCount] = strtok(p_from, "-");
	while((itemList[++itemCount] = strtok(NULL, "-")));

	for(int i=0; i<itemCount; i++)
	{
		strcat(p_to, itemList[i]);
	}

	strncpy(tempStr, p_to, UUID_VERSION_POS);
//	printf(" tempStr1: %s\n", tempStr);
	strcat(tempStr, &p_to[13]);
//	printf(" tempStr2: %s\n", tempStr);
	strncpy(p_to, tempStr, KEYSTONE_ID_LEN);

	return strBuff;
}

int CKeystoneInfo::GetEndpointInfo(std::vector<KeystoneEndpoint> &_vecEndp,
                                   char *_tbName,
                                   bool _vmode)
{
	printf("--- keystone::%s --------------------------------------------------\n", _tbName);

	int		count=0;
	int		nRowCount; 
	int		fields;
	char	query[1024];
	char	uuid[UUID_LEN+1];
	KeystoneEndpoint	nEndp;

	memset(query, 0x00, sizeof(query));
	sprintf(query, "select * from %s", _tbName);

	nRowCount = m_pcDbConnect_->ExecuteSQL((char *)query);
	if (nRowCount == ITF_ERROR)
	{
		printf(" Table [%s] info get failed\n\n", _tbName);
		return ITF_ERROR;
	}
	else if (nRowCount == 0)
	{
		printf(" Table [%s] has no more rows\n\n", _tbName);
		return ITF_ERROR;
	}

	m_pResult = m_pcDbConnect_->GetDBRes();
	fields    = mysql_num_fields(m_pResult);

	while((row = mysql_fetch_row(m_pResult)))
	{
		memset(&nEndp, 0x00, sizeof(KeystoneEndpoint));

#ifdef _PRINT_ALLROWS
		for(int cnt = 0 ; cnt < fields ; ++cnt)
		{
			printf(" %s ||", row[cnt]);
		}
		printf("\n");
#endif
		unsigned int rowIdx = 0;

		strncpy(nEndp.id,        row[rowIdx++], ID_SIZE_K);
		rowIdx++;
		strncpy(nEndp.interface, row[rowIdx++], INF_SIZE);
		rowIdx++;
		strncpy(nEndp.url,       row[rowIdx++], URL_SIZE);

		if (_vmode == true)
		{
			printf(" %2d:", count++);
			printf(" [ID] "CYAN"%s "RESET"||", nEndp.id );
			printf(" [INTERFACE] "CYAN"%s "RESET"||\n", nEndp.interface);
			printf("     [URL] "CYAN"%s "RESET"\n", nEndp.url);
		}
		else
		{
			memset(uuid, 0x00, sizeof(uuid));
			sprintf(uuid, "%s", ConvertIDToUUID(nEndp.id));

			printf(" %2d:", count++);
			printf(" [KEYSTONE ID] "CYAN"%s "RESET"-->", nEndp.id );
			printf(" [UUID] "CYAN"%s "RESET"\n", uuid);
			printf("      --> [VTN ID] "CYAN"%s "RESET"\n", ConvertUUIDToVTNKey(uuid) );
		}

		_vecEndp.push_back(nEndp);
	}

	if (_vmode == false)
		printf(" IDs are changed into UUID\n\n");
	else
		printf("\n");

	return ITF_OK; 
}

int CKeystoneInfo::GetProjectInfo(std::vector<KeystoneProject> &_vecProj,
                                  char *_tbName,
                                  bool _vmode)
{
	printf("--- keystone::%s --------------------------------------------------\n", _tbName);

	int		count=0;
	int		nRowCount;
	int		fields;
	char	query[1024];
	char	uuid[UUID_LEN+1];
	KeystoneProject	nProj;

	memset(query, 0x00, sizeof(query));
	sprintf(query, "select * from %s", _tbName);

	nRowCount = m_pcDbConnect_->ExecuteSQL((char*)query);
	if (nRowCount == ITF_ERROR)
	{
		printf(" Table [%s] info get failed\n\n", _tbName);
		return ITF_ERROR;
	}
	else if (nRowCount == 0)
	{
		printf(" Table [%s] has no more rows\n\n", _tbName);
		return ITF_ERROR;
	}

	m_pResult = m_pcDbConnect_->GetDBRes();
	fields    = mysql_num_fields(m_pResult);

	while((row = mysql_fetch_row(m_pResult)))
	{
		memset(&nProj, 0x00, sizeof(KeystoneProject));

#ifdef _PRINT_ALLROWS
		for(int cnt = 0 ; cnt < fields ; ++cnt)
		{
			printf(" %s ||", row[cnt]);
		}
		printf("\n");
#endif
		unsigned int rowIdx = 0;

		strncpy(nProj.id,   row[rowIdx++], ID_SIZE_K);

		strncpy(nProj.name, row[rowIdx++], NAME_SIZE_K);

		if (_vmode == true)
		{
			printf(" %2d:", count++);
			printf(" [ID] "CYAN"%s "RESET"||", nProj.id );
			printf(" [NAME] "CYAN"%s "RESET"||\n", nProj.name);
		}
		else
		{
			memset(uuid, 0x00, sizeof(uuid));
			sprintf(uuid, "%s", ConvertIDToUUID(nProj.id));

			printf(" %2d:", count++);
			printf(" [KEYSTONE ID] "CYAN"%s "RESET"-->", nProj.id );
			printf(" [UUID] "CYAN"%s "RESET"\n", uuid );
			printf("      --> [VTN ID] "CYAN"%s "RESET"\n", ConvertUUIDToVTNKey(uuid) );
		}

		_vecProj.push_back(nProj);
	}

	if (_vmode == false)
		printf(" IDs are changed into UUID\n\n");
	else
		printf("\n");

	return ITF_OK;
}

int CKeystoneInfo::GetTokenInfo(std::vector<KeystoneToken> &_vecToken,
                                char *_tbName,
                                bool _vmode)
{
	printf("--- keystone::%s --------------------------------------------------\n", _tbName);

	int		count=0;
	int		nRowCount;
	int		fields;
	char	query[1024];
	char	uuid[UUID_LEN+1];
	KeystoneToken	nToken;

	memset(query, 0x00, sizeof(query));
	sprintf(query, "select * from %s", _tbName);

	nRowCount = m_pcDbConnect_->ExecuteSQL((char*)query);
	if (nRowCount == ITF_ERROR)
	{
		printf(" Table [%s] info get failed\n\n", _tbName);
		return ITF_ERROR;
	}
	else if (nRowCount == 0)
	{
		printf(" Table [%s] has no more rows\n\n", _tbName);
		return ITF_ERROR;
	}

	m_pResult = m_pcDbConnect_->GetDBRes();
	fields    = mysql_num_fields(m_pResult);

	while((row = mysql_fetch_row(m_pResult)))
	{
		memset(&nToken, 0x00, sizeof(KeystoneToken));

		unsigned int rowIdx=0;

		strncpy(nToken.id,      row[rowIdx++], ID_SIZE_K);
		rowIdx++;
		strncpy(nToken.extra,   row[rowIdx++], EXTRA_SIZE);
		rowIdx += 2;
		strncpy(nToken.user_id, row[rowIdx++], ID_SIZE_K);

		if (_vmode == true)
		{
			printf(" %3d:", count++);
			printf(" [ID] "CYAN"%s "RESET"||", nToken.id );
			printf(" [USER_ID] "CYAN"%s "RESET"||\n", nToken.user_id);
			printf("      [EXTRA] "CYAN"%s "RESET"||\n", nToken.extra);
		}
		else
		{
			memset(uuid, 0x00, sizeof(uuid));
			sprintf(uuid, "%s", ConvertIDToUUID(nToken.id));

			printf(" %3d:", count++);
			printf(" [KEYSTONE ID] "CYAN"%s "RESET"-->", nToken.id );
			printf(" [UUID] "CYAN"%s "RESET"\n", uuid );
			printf("       --> [VTN ID] "CYAN"%s "RESET"\n", ConvertUUIDToVTNKey(uuid) );
		}

		_vecToken.push_back(nToken);
	}

	if (_vmode == false)
		printf(" IDs are changed into UUID\n\n");
	else
		printf("\n");

	return ITF_OK;
}

int CKeystoneInfo::GetDBInfo(char *_tbName)
{
	printf("--- keystone::%s --------------------------------------------------\n", _tbName);

	int		nRowCount;
	int		fields;
	char	query[1024];

	memset(query, 0x00, sizeof(query));
	sprintf(query, "select * from %s", _tbName);

	nRowCount = m_pcDbConnect_->ExecuteSQL((char*)query);
	if (nRowCount == ITF_ERROR)
	{
		printf(" Table [%s] info get failed\n\n", _tbName);
		return ITF_ERROR;
	}
	else if (nRowCount == 0)
	{
		printf(" Table [%s] has no more rows\n\n", _tbName);
		return ITF_ERROR;
	}

	m_pResult = m_pcDbConnect_->GetDBRes();
	fields    = mysql_num_fields(m_pResult);

	while((row = mysql_fetch_row(m_pResult)))
	{
		for(int cnt = 0 ; cnt < fields ; ++cnt)
		{
			printf(" %s ||", row[cnt]);
		}
		printf("\n");
	}
	printf("\n");

	return ITF_OK;
}

int CKeystoneInfo::ShowTableInfo(void)
{
	printf("--- nova::table list --------------------------------------------------\n");

	int     nRowCount;
	int     fields;
	char    query[1024];

	memset(query, 0x00, sizeof(query));
	sprintf(query, "show tables");

	nRowCount = m_pcDbConnect_->ExecuteSQL((char*)query);
	if (nRowCount == ITF_ERROR)
	{
		printf(" Table info get failed\n\n");
		return ITF_ERROR;
	}
	else if (nRowCount == 0)
	{
		printf(" Table has no more rows\n\n");
		return ITF_ERROR;
	}

	m_pResult = m_pcDbConnect_->GetDBRes();
	fields    = mysql_num_fields(m_pResult);

	while((row = mysql_fetch_row(m_pResult)))
	{
		for(int cnt = 0 ; cnt < fields ; ++cnt)
		{
			printf(" %s ||", row[cnt]);
		}
		printf("\n");
	}
	printf("\n");

	return ITF_OK;
}