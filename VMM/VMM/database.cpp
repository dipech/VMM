#define _CRT_SECURE_NO_WARNINGS

#include "database.h"
#include "defines.h"
#include "functions.h"

Database::Database()
{
	db = NULL;
	isAssigned = false;
}
RC_RESULT Database::AssignDatabase(char *_chooseFolderDialogTitle, char *_path)
{
	RC_RESULT result = RC_ERROR;

	char databaseFolder[MAX_PATH];
	ZeroMemory(databaseFolder,sizeof(char)*MAX_PATH);
	
	char *fullPath = new char[MAX_PATH];
	ZeroMemory(fullPath, sizeof(char)*MAX_PATH);

	char copyAfter_fullPath[] = "\\data1.db";
	char copyUntil_fullPath[] = "file:///";

	strcat(fullPath, copyUntil_fullPath);
	strcat(fullPath, _path);
	strcat(fullPath, copyAfter_fullPath);

	for (int i = 0; i < MAX_PATH; i++)
	{
		if (fullPath[i] == '\\')
			fullPath[i] = '/';
	}
	memcpy(databaseFolder, fullPath, sizeof(char)*MAX_PATH);

	int resultCode_openDB = NULL;
	resultCode_openDB = sqlite3_open_v2
		((const char*)databaseFolder, &db,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI, 0);

	if (resultCode_openDB == SQLITE_OK)
	{
		isAssigned = true;
		pathToDatabaseFolder = databaseFolder;
		result = RC_SUCCESS;
	}
	else
	{
		MessageBox(0, "Cannot open database", "Error", 0);
	}
	
	return result;
}
RC_RESULT Database::AssignDatabase(char *_chooseFolderDialogTitle)
{
	RC_RESULT result = RC_ERROR;

	char databaseFolder[MAX_PATH];
	ZeroMemory(databaseFolder, sizeof(char)*MAX_PATH);

	memcpy(databaseFolder, pathToDatabaseFolder.c_str(), sizeof(char)*pathToDatabaseFolder.size());

	int resultCode_openDB = NULL;
	resultCode_openDB = sqlite3_open_v2
		((const char*)databaseFolder, &db,
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI, 0);

	if (resultCode_openDB == SQLITE_OK)
	{
		isAssigned = true;
		pathToDatabaseFolder = databaseFolder;
		result = RC_SUCCESS;
	}

	return result;
}
void Database::CloseDatabase()
{
	sqlite3_close_v2(db);
	db = NULL;
	isAssigned = false;
}
void Database::InsertModel(HWND _hMainWindow)
{
	char szFileName[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hMainWindow;
	ofn.lpstrFilter = "Voxel Model Files (*.cub)\0*.cub\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";
	if (GetOpenFileName(&ofn))
	{
		FILE* fileOfModel = NULL;
		fileOfModel = fopen((ofn).lpstrFile, "rb");

		if (fileOfModel!=NULL)
		{
			int x_size = 0, y_size = 0, z_size = 0;

			fread(&z_size, 4, 1, fileOfModel);
			fread(&x_size, 4, 1, fileOfModel);
			fread(&y_size, 4, 1, fileOfModel);

			byte *blobToDB;
			int blobToDBSize = 12+x_size*y_size*z_size*3;
			blobToDB = (byte*)malloc(blobToDBSize);
			ZeroMemory(blobToDB, blobToDBSize);

			int offset = 12;
			memcpy(blobToDB, &z_size, 4);
			memcpy(blobToDB+4, &x_size, 4);
			memcpy(blobToDB+8, &y_size, 4);

			for (int y = 0; y<y_size; y++)
			{
				for (int x = 0; x<x_size; x++)
				{
					for (int z = 0; z<z_size; z++)
					{
						DWORD asd;
						fread(&asd, 3, 1, fileOfModel);
						memcpy(blobToDB+offset, &asd, 3);
						offset += 3;
					}
				}
			}

			CodeData(blobToDB, blobToDBSize);

			int start_filename = ofn.nFileOffset;
			int sizeof_filename = strlen(ofn.lpstrFile)-ofn.nFileOffset;
			char *sub_str = (char*)malloc(sizeof(char)*(sizeof_filename+1));
			ZeroMemory(sub_str, sizeof(char)*(sizeof_filename+1));

			int j = 0;
			for (int i = start_filename; i<start_filename+sizeof_filename; i++)
			{
				sub_str[j] = ofn.lpstrFile[i];
				j++;
			}

			char* key_ID;
			key_ID = (char*)malloc((sizeof_filename+1)*sizeof(char));
			ZeroMemory(key_ID, (sizeof_filename+1)*sizeof(char));
			key_ID = sub_str;

			sqlite3_stmt *stmt_delete;
			char *mysql_querry_delete;
			mysql_querry_delete = (char*)malloc(sizeof(char)*strlen(key_ID)+sizeof(char)*31);
			ZeroMemory(mysql_querry_delete, sizeof(char)*strlen(key_ID)+sizeof(char)*31);
			strcat(mysql_querry_delete, "DELETE FROM blobs WHERE key = \"");
			strcat(mysql_querry_delete, (const char*)key_ID);
			strcat(mysql_querry_delete, "\"");
			int hr_prepare_to_delete = sqlite3_prepare(db, mysql_querry_delete, -1, &stmt_delete, 0);

			int hr_delete_step = sqlite3_step(stmt_delete);

			const char *zSql = "INSERT INTO blobs(key, value) VALUES(?, ?)";
			sqlite3_stmt *pStmt;
			int rc;

			do
			{
				rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);

				int result_bind_text = sqlite3_bind_text(pStmt, 1, (const char*)key_ID, -1, SQLITE_STATIC);
				int result_bind_blob = sqlite3_bind_blob(pStmt, 2, blobToDB, blobToDBSize, SQLITE_STATIC);

				rc = sqlite3_step(pStmt);
				rc = sqlite3_finalize(pStmt);
			} while (rc==SQLITE_SCHEMA);
			fclose(fileOfModel);
		}
	}
}
void Database::ExportModel(HWND _hMainWindow)
{
	if (openedModelName.length() != 0)
	{
		char modelName[MAX_PATH];
		ZeroMemory(modelName,sizeof(char)*MAX_PATH);
		sprintf_s(modelName,"%s", (LPSTR)openedModelName.c_str());

		char modelName1[MAX_PATH];
		memcpy(modelName1, modelName, sizeof(char)*MAX_PATH);

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = _hMainWindow;
		ofn.lpstrFilter = "Voxel Model Files (*.cub)\0*.cub\0All Files (*.*)\0*.*\0";
		ofn.lpstrFile = modelName1;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		ofn.lpstrDefExt = "txt";

		if (GetSaveFileName(&ofn))
		{
			FILE *fileOfModel = NULL;
			fopen_s(&fileOfModel,(const char*)ofn.lpstrFile, "wb");

			if (fileOfModel!=NULL)
			{
				DWORD ***voxels = 0;
				int xSize = 0, ySize = 0, zSize = 0;				
				GetModel(&voxels, &xSize, &ySize, &zSize, modelName);
				openedModelName = modelName;
				fwrite(&zSize, 4, 1, fileOfModel);
				fwrite(&xSize, 4, 1, fileOfModel);
				fwrite(&ySize, 4, 1, fileOfModel);

				for (int y = 0; y<ySize; y++)
				{
					for (int x = 0; x<xSize; x++)
					{
						for (int z = 0; z<zSize; z++)
						{
							DWORD asd = voxels[y][xSize-x-1][z];
							SwapCol(&asd);
							fwrite(&asd, 3, 1, fileOfModel);
						}
					}
				}

				for (int y = 0; y<ySize; y++)
				{
					for (int x = 0; x<xSize; x++)
						delete[] voxels[y][x];
					delete[] voxels[y];
				}
				delete[] voxels;
				voxels = nullptr;
				fclose(fileOfModel);
			}
		}
	}
}
void Database::UpdateModel(DWORD ***_voxels, int _xSize, int _ySize, int _zSize)
{
	if (openedModelName.length()!=0)
	{
		byte *blobToDB;
		int blobToDBSize = 12+_xSize*_ySize*_zSize*3;
		blobToDB = (byte*)malloc(blobToDBSize);
		ZeroMemory(blobToDB, blobToDBSize);

		int offset = 12;
		memcpy(blobToDB, &_zSize, 4);
		memcpy(blobToDB+4, &_xSize, 4);
		memcpy(blobToDB+8, &_ySize, 4);
		for (int y = 0; y<_ySize; y++)
		{
			for (int x = 0; x<_xSize; x++)
			{
				for (int z = 0; z<_zSize; z++)
				{
					DWORD asd = _voxels[y][_xSize-x-1][z];
					SwapCol(&asd);
					memcpy(blobToDB+offset, &asd, 3);
					offset += 3;
				}
			}
		}
		CodeData(blobToDB, blobToDBSize);

		byte* key_ID;
		key_ID = (byte*)openedModelName.c_str();

		sqlite3_stmt *stmt_delete;
		char *mysql_querry_delete;
		mysql_querry_delete = (char*)malloc(sizeof(unsigned char)*(unsigned char)key_ID+sizeof(char)*31);
		ZeroMemory(mysql_querry_delete, sizeof(unsigned char)*(unsigned char)key_ID+sizeof(char)*31);
		strcat(mysql_querry_delete, "DELETE FROM blobs WHERE key = \"");
		strcat(mysql_querry_delete, (const char*)key_ID);
		strcat(mysql_querry_delete, "\"");
		int hr_prepare_to_delete = sqlite3_prepare(db, mysql_querry_delete, -1, &stmt_delete, 0);

		int hr_delete_step = sqlite3_step(stmt_delete);

		const char *zSql = "INSERT INTO blobs(key, value) VALUES(?, ?)";
		sqlite3_stmt *pStmt;
		int rc;

		do
		{
			rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);

			int result_bind_text = sqlite3_bind_text(pStmt, 1, (const char*)key_ID, -1, SQLITE_STATIC);
			int result_bind_blob = sqlite3_bind_blob(pStmt, 2, blobToDB, blobToDBSize, SQLITE_STATIC);

			rc = sqlite3_step(pStmt);
			rc = sqlite3_finalize(pStmt);
		} while (rc==SQLITE_SCHEMA);
	}	
}
void Database::DeleteModel()
{

}
void Database::GetListOfModels()
{
	sqlite3_stmt* pStmt;
	int i = 0;
	int resultCode = 0;
	char sql_querry[] = "SELECT key FROM blobs";
	do
	{
		sqlite3_prepare(db, sql_querry, -1, &pStmt, 0);
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			char key[MAX_PATH];
			sprintf(key, "%s", sqlite3_column_text(pStmt, 0));
			string str_key = key;
			i++;
			modelsNames.push_back(str_key);
		}
		resultCode = sqlite3_finalize(pStmt);
	} while (resultCode == SQLITE_SCHEMA);
}
void Database::GetModel(DWORD ****_voxels, int *xSize_, int *ySize_, int *zSize_, char *_key)
{
	byte* key_ID = (byte*)_key;

	const char *sql_querry = "SELECT value FROM blobs WHERE key = ?";
	sqlite3_stmt *statement;
	int rc;

	const char *zKey = (const char*)key_ID;
	int pnBlob = 0;

	byte *loadedData = 0;

	do
	{
		rc = sqlite3_prepare(db, sql_querry, -1, &statement, 0);
		if (rc == SQLITE_OK)
		{
			sqlite3_bind_text(statement, 1, zKey, -1, SQLITE_STATIC);

			rc = sqlite3_step(statement);
			if (rc == SQLITE_ROW)
			{

				pnBlob = sqlite3_column_bytes(statement, 0);
				loadedData = (unsigned char *)malloc(pnBlob);
				memcpy(loadedData, (void *)sqlite3_column_blob(statement, 0), pnBlob);
			}
			rc = sqlite3_finalize(statement);
		}		
	} while (rc == SQLITE_SCHEMA);

	DecodeData(loadedData, pnBlob);

	int xSize = 0, ySize = 0, zSize = 0;
	memcpy(&zSize, loadedData, 4);
	memcpy(&xSize, loadedData + 4, 4);
	memcpy(&ySize, loadedData + 8, 4);

	(*_voxels) = new DWORD **[ySize];
	for (int y = 0; y < ySize; y++)
	{
		(*_voxels)[y] = new DWORD *[xSize];
		for (int x = 0; x < xSize; x++)
		{
			(*_voxels)[y][x] = new DWORD[zSize];
		}
	}	

	int offset = 12;
	for (int y = 0; y<ySize; y++)
	{
		for (int x = xSize - 1; x>-1; x--)
		{
			for (int z = 0; z < zSize; z++)
			{
				DWORD color;
				memcpy(&color, loadedData + offset, 3);
				SwapCol(&color);
				(*_voxels)[y][x][z] = color;
				offset += 3;
			}
		}
	}
	*xSize_ = xSize;
	*ySize_ = ySize;
	*zSize_ = zSize;
}
void Database::ChangeCurrentModelName()
{

}
RC_RESULT GetDatabaseFolder(char *folder, char *_chooseFolderDialogTitle)
{
	ZeroMemory(folder,sizeof(char)*MAX_PATH);
	RC_RESULT result = RC_ERROR;

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _chooseFolderDialogTitle;
	LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
	if ( pidl != 0 )
	{
		TCHAR path[MAX_PATH];
		if ( SHGetPathFromIDList ( pidl, path ) )
		{
			char *fullPath = new char[MAX_PATH];
			ZeroMemory(fullPath,sizeof(char)*MAX_PATH);

			char copyAfter_fullPath[]="\\data1.db";
			char copyUntil_fullPath[]="file:///";

			strcat(fullPath,copyUntil_fullPath);
			strcat(fullPath,path);
			strcat(fullPath,copyAfter_fullPath);

			for (int i=0; i<MAX_PATH;i++)
			{
				if (fullPath[i] == '\\')
					fullPath[i] =  '/';
			}
			memcpy(folder,fullPath, sizeof(char)*MAX_PATH);
			result = RC_SUCCESS;
		}

		// free memory used
		IMalloc * imalloc = 0;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
		{
			imalloc->Free ( pidl );
			imalloc->Release ( );
		}
	}
	return result;
}
void Database::CodeData(unsigned char *_arr, int size)
{
	static int offsetLookupTable[] = 
	{
		0x1092, 0x254F, 0x348, 0x14B40, 0x241A, 0x2676, 0x7F, 0x9, 0x250B,
		0x18A, 0x7B, 0x12E2, 0x7EBC, 0x5F23, 0x981, 0x11, 0x85BA, 0x0A566,
		0x1093, 0x0E, 0x2D266, 0x7C3, 0x0C16, 0x76D, 0x15D41, 0x12CD,
		0x25, 0x8F, 0x0DA2, 0x4C1B, 0x53F, 0x1B0, 0x14AFC, 0x23E0, 0x258C,
		0x4D1, 0x0D6A, 0x72F, 0x0BA8, 0x7C9, 0x0BA8, 0x131F, 0x0C75C7, 0x0D
	};

	for ( int i = 0 ; i < size ; i++ )
	{
		_arr[ i ] = ( byte )( -1 - _arr[ i ] );
	}

	for ( int currOff = 0 ; currOff < size ; currOff++ )
	{
		int offset = ( currOff + offsetLookupTable[ currOff % 44 ] ) % size;

		byte temp = _arr[ currOff ];
		_arr[ currOff ] = _arr[ offset ];
		_arr[ offset ] = temp;
	}
}
void Database::DecodeData(unsigned char *_arr, int size)
{
	static int offsetLookupTable[] = 
	{
		0x1092, 0x254F, 0x348, 0x14B40, 0x241A, 0x2676, 0x7F, 0x9, 0x250B,
		0x18A, 0x7B, 0x12E2, 0x7EBC, 0x5F23, 0x981, 0x11, 0x85BA, 0x0A566,
		0x1093, 0x0E, 0x2D266, 0x7C3, 0x0C16, 0x76D, 0x15D41, 0x12CD,
		0x25, 0x8F, 0x0DA2, 0x4C1B, 0x53F, 0x1B0, 0x14AFC, 0x23E0, 0x258C,
		0x4D1, 0x0D6A, 0x72F, 0x0BA8, 0x7C9, 0x0BA8, 0x131F, 0x0C75C7, 0x0D
	};

	for ( int currOff = size - 1 ; currOff >= 0 ; currOff-- )
	{
		int offset = ( currOff + offsetLookupTable[ currOff % 44 ] ) % size;

		byte temp = _arr[ currOff ];
		_arr[ currOff ] = _arr[ offset ];
		_arr[ offset ] = temp;
	}

	for ( int i = 0 ; i < size ; i++ )
	{
		_arr[ i ] = ( byte )( -1 - _arr[ i ] );
	}
}
bool Database::IsAssigned()
{
	return isAssigned;
}
vector<string> Database::GetModelsNames()
{
	return modelsNames;
}
string Database::GetPathToDatabaseFolder()
{
	return pathToDatabaseFolder;
}
void Database::SetPathToDatabaseFolder(string _path)
{
	pathToDatabaseFolder = _path;
}
void Database::SetOpenedModelName(string _name)
{
	openedModelName = _name;
}
string Database::GetOpenedModelName()
{
	return openedModelName;
}