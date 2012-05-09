#include <vector>
#include <iostream>

#include "Simulator.hpp"
#include "Parameter.hpp"

using namespace std;

Simulator::Simulator(Environment * environment)
{
	env = environment;
}

bool Simulator::addPWordAgent(WordAgent & pWordAgent)
{
        //cout<<"simu ag id "<<pWordAgent->getID()<<endl;
	env->addPWordAgent(pWordAgent);
	return true;
}

bool Simulator::run(const Sentence & sen, const std::vector<int> & fa)
{
        cout<<"running..."<<endl;
        /*reset interating objects*/
	bool hasRun = true;
	int c;
	std::vector<std::map<int, WordAgent> > agents = env->getAgents();
        /*for(size_t  j = 0; j < agents.size(); j++)
        {
                cout<<agents[j].size()<<" ";
        }
        cout<<endl;
        cin>>c;
        */

	/*for(size_t m = 0; m < agents.size(); m++)
	{
	        //cout<<"set size is "<<agents.size()<<endl;
	        if(agents[m].size() != 0)
	        {
	                cout<<"m "<<m<<" size is "<<agents[m].size()<<endl;
	                cin>>c;

	                set<WordAgent *>::iterator it = agents[m].begin();
	                while(it != agents[m].end())
	                {
	                        cout<<"id is "<<(*it)->getID()<<" , cate is "<<(*it)->getCategory()<<endl;

	                        it++;
                        }

                        cout<<"**"<<endl;
                        cin>>c;
                }
        }

        int b;
        cin>>b;
        */

	/*termination consitions:
	(1) All antigens are killed;
	(2) All B cells are active;
	*/
	//cout<<endl;
	int a;
	while(hasRun){
	        hasRun = false;
	        //cout<<"ag number is "<<env->getAntigenNum()<<endl;
	        //cin>>a;
                for(size_t i = 0; i < agents.size(); i++){
			for(map<int,WordAgent>::iterator it = agents[i].begin(); it != agents[i].end(); it++)
			{
			        //cout<<(*it)->getID()<<" "<<(*it)->getCategory()<<" ";

			        it->second.run();
				if((env->getAntigenNum() != 0)){
					hasRun = true;
				}
			}
		}
		//cin>>a;

		//cout<<"ag number is "<<env->getAntigenNum()<<endl;
	}

	return true;
}



