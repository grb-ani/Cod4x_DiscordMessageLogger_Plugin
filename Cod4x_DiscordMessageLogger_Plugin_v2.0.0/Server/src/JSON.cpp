#include "JSON.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cjson/cJSON.h>

std::shared_ptr<Json::discordData> Json::dData = nullptr;

Json::Json() {
	jsonCfgFile = fopen("./config.json", "r");
	if(jsonCfgFile == nullptr) {
		fprintf(stderr, "Error occurred on opening config file, exiting...\n");
		exit(EXIT_FAILURE);

	}

	fseek(jsonCfgFile, 0L, SEEK_END);
	configLen = ftell(jsonCfgFile);
	if (configLen > 1024) {
		fprintf(stderr, "Error occurred on reading configuration file: File is too large, exiting...\n");
		exit(EXIT_FAILURE);

	}

	rewind(jsonCfgFile);

	rawJson = (char*)malloc((configLen + 1) * sizeof(char));
	if(rawJson == nullptr) {
		fprintf(stderr, "Error occurred on allocating memory for storing raw json, pointer is null, exiting...\n");
		exit(EXIT_FAILURE);

	}

	readLen = fread(rawJson, sizeof(char), configLen, jsonCfgFile);
	if(readLen != configLen) {
		fprintf(
				stderr,
				"Error occurred on reading the config file, read size ( %zu ) doesn't match the actual config file size( %zu ), exiting...\n",
				readLen,
				configLen
		);
		exit(EXIT_FAILURE);

	}

	rawJson[readLen + 1] = '\0';
	fclose(jsonCfgFile);

	printf("NOTICE: Configuration file has read successfully, raw configuration file is:\n%s\n", rawJson);

	ptrErr = nullptr;
	jsonCfg = cJSON_ParseWithOpts((const char*)rawJson, &ptrErr, 1);
	if(jsonCfg == nullptr) {
		errOffset = ptrErr - rawJson;

		startOffset = (errOffset > 10) ? errOffset - 10 : 0;
		offsetLen = (errOffset + 10ULL < strlen(rawJson)) ? 10 : strlen(rawJson) - startOffset;
		fprintf(
				stderr,
				"Error occurred on parsing the json configuration file,"
				"the error occurred on:\n%.*s\nexiting...\n",
				(int)offsetLen,
				rawJson + startOffset
		);
		exit(EXIT_FAILURE);

	}

}

void Json::readDataToStruct() {
	discordInfo = cJSON_GetObjectItemCaseSensitive(jsonCfg, "Discord Information");
	if(discordInfo == nullptr || !cJSON_IsObject(discordInfo)) {
		fprintf(stderr, "Error occurred on getting Discord Information Object, exiting...\n");
		cJSON_Delete(jsonCfg);
		exit(EXIT_FAILURE);

	}

	botToken = cJSON_GetObjectItemCaseSensitive(discordInfo, "Bot Token");
	if(botToken == nullptr || !cJSON_IsString(botToken) || botToken->valuestring == nullptr) {
		fprintf(stderr, "Error occurred on getting bot token Object, exiting...\n");
		cJSON_Delete(jsonCfg);
		exit(EXIT_FAILURE);

	}

	channelId = cJSON_GetObjectItemCaseSensitive(discordInfo, "Channel ID");
	if(channelId == nullptr || !cJSON_IsString(channelId) || channelId->valuestring == nullptr) {
		fprintf(stderr, "Error occurred on getting channel ID Object, exiting...\n");
		cJSON_Delete(jsonCfg);
		exit(EXIT_FAILURE);

	}

	if(dData == nullptr) {
		fprintf(stderr, "Error occurred on parsing discord data to structure: Structure is a nullptr, exiting...\n");
		cJSON_Delete(jsonCfg);
		exit(EXIT_FAILURE);

	}

	char* endPtr = nullptr;
	uint64_t cIdresult = strtoull(channelId->valuestring, &endPtr, 10);
	if(cIdresult == ULLONG_MAX) {
		fprintf(stderr, "Error occurred on reading the Channel ID: The length is wider than expected, exiting...\n");
		cJSON_Delete(jsonCfg);
		free(discordInfo);
		free(botToken);
		free(channelId);
		exit(EXIT_FAILURE);

	} else if(endPtr == channelId->valuestring) {
		fprintf(stderr, "Error occurred on reading the Channel ID: No bytes has been read, exiting...\n");
		cJSON_Delete(jsonCfg);
		free(discordInfo);
		free(botToken);
		free(channelId);
		exit(EXIT_FAILURE);

	}

	printf("NOTICE: The converted number is: %llu\n", cIdresult);

	dData->botToken = botToken->valuestring;
	dData->channelID = cIdresult;

	free(discordInfo);
	free(botToken);
	free(channelId);

}
