#include "Signal.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <memory>
#include <cstddef>

std::shared_ptr<SigInt> SigInt::instance = nullptr;

void SigInt::sCallback(int sigNum) {
	printf("NOTICE: SIGINT received, exiting...\n");

	if(instance == nullptr) {
		fprintf(stderr, "Error occurred on calling sCallback: Instance is a nullptr");
		exit(EXIT_FAILURE);

	}

	*instance->qRun = 0;

}

void SigInt::regSig() {
	memset(&sigIntStruct, 0, sizeof(sigIntStruct));
	sigIntStruct.sa_handler = SigInt::sCallback;
	sigIntStruct.sa_flags = 0;

	rValue = sigaction(SIGINT, &sigIntStruct, NULL);
	printf("NOTICE: sigaction is returning: %i\n", rValue);

	if(rValue == -1) {
		fprintf(stderr, "Error occurred on calling sigaction: %s", strerror(errno));
		exit(EXIT_FAILURE);

	}

	printf("NOTICE: Registered signal handler.\n");

}
