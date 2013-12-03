#pragma once

#include <iostream>
#include <boost\mpi.hpp>
#include <boost\asio\ip\host_name.hpp>
#include <boost\thread.hpp>

namespace mpi = boost::mpi;

class MemStruct
{
public:
	MemStruct(std::string hnName = "", bool procHN = true);
	~MemStruct(void);

	unsigned GetConcurrency(int nNum = -1);
	void DebugPrint(void);

	std::vector<unsigned> concurrency;

protected:
	bool procOnHN;
	mpi::communicator world;
	unsigned hnID;

private:
	
	std::string hnNName;	

	void BuildClusInfo(void);
	void BuildClusNames(void);
	void BuildClusConcurrency(void);
	void ConsumeMsg(std::vector<int> msg);
};

