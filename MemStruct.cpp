#include "MemStruct.h"

MemStruct::MemStruct(std::string hnName, bool procHN)
{
	hnNName = hnName;
	procOnHN = procHN;
	hnID = 0;

	BuildClusInfo();

	/*if(world.rank() == hnID)
		DebugPrint();*/
}

MemStruct::~MemStruct(void)
{
}

void MemStruct::DebugPrint(void)
{
	for(int i=0;i<world.size();i++)
	{
		std::cout << GetConcurrency(i) << std::endl;
	}
}

void MemStruct::BuildClusInfo(void)
{
	BuildClusNames();
	BuildClusConcurrency();
}

void MemStruct::BuildClusNames(void)
{
	std::string mName = boost::asio::ip::host_name();

	std::vector<std::string> hNames;

	if(world.rank() == 0)
	{
		mpi::gather(world, mName, hNames, 0);

		std::cout << "Processing on nodes:" << std::endl;

		for(unsigned i=0;i<hNames.size();i++)
			std::cout << "Process " << i << ":" << hNames[i] << std::endl;


	}
	else
	{
		mpi::gather(world, mName, 0);
	}

	int hnMsg;
	std::vector<int> msgav;

	if(world.rank() == 0)
	{
		hnMsg = 1;
		unsigned nhnID = 0;

		//Check for duplicates
		for(unsigned i=0;i<hNames.size();i++)
		{
			for(unsigned j=i+1;j<hNames.size();j++)
			{
				if(hNames[i] == hNames[j])
				{
					std::cerr << "Duplicate hostname detected!  Ensure program is running in blade/node mode."  << std::endl;
					hnMsg = -1;
				}
			}

			if(hNames[i] == hnNName)
			{
				nhnID = i;
			}
		}

		msgav.push_back(hnMsg);

		if(hnMsg == -1)
			msgav.push_back(-1);

		if(hnMsg == 1)
			msgav.push_back(nhnID);
	}

	mpi::broadcast(world, msgav, 0);
	ConsumeMsg(msgav);
}

void MemStruct::BuildClusConcurrency(void)
{
	unsigned con = boost::thread::hardware_concurrency();

	if(world.rank() == hnID)
	{
		mpi::gather(world, con, concurrency, hnID);
	}
	else
	{
		mpi::gather(world, con, hnID);
	}
}

void MemStruct::ConsumeMsg(std::vector<int> msg)
{
	/* 
	-1 = exit due to error
	0 = exit successfully (having completed)
	1 = headnode change
	*/

	switch(msg[0])
	{
	case -1:
		exit(msg[1]);
		break;

	case 0:
		exit(0);
		break;

	case 1:
		if(world.rank() == hnID && hnID != msg[1])
			std::cout << "Process " << world.rank() << ": Ceding controller status to Process " << msg[1] << std::endl;
		else if(world.rank() == msg[1] && world.rank() != hnID)
			std::cout << "Process " << world.rank() << ": Granted controller status by Process " << hnID << std::endl;
		else
			std::cout << "Process " << world.rank() << ": Recognising Process " << msg[1] << " as new controller" << std::endl;

		hnID = msg[1];
		break;
	}
}

unsigned MemStruct::GetConcurrency(int nNum)
{
	if(world.rank() != hnID)
	{
		std::cerr << "Attempt to call GetConcurrency method from non-head node " << boost::asio::ip::host_name() << std::endl;
		return 0;
	}

	if(concurrency.size() < 1)
	{
		std::cerr << "Calling GetConcurrency with an unpopulated list, or none available." << std::endl;
		return 0;
	}

	unsigned rv = 0;

	if(nNum < 0)
	{
		for(size_t i=0;i<concurrency.size();i++)
		{
			if(i == hnID && procOnHN == false)
			{
				//skip
			}
			else
			{
				rv += concurrency[i];
			}
		}

		if(rv == 0)
		{
			std::cerr << "Calling GetConcurrency with only a non-processed headnode available." << std::endl;
		}
	}
	else
	{
		if(concurrency.size() < nNum)
		{
			std::cerr << "Calling GetConcurrency on a node which does not exist." << std::endl;
			return 0;
		}

		return concurrency[nNum];

	}

	return rv;
}