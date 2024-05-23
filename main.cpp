#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

template<typename T>
class Generator
{
public:
    Generator(std::uint32_t fullSequenceArray, std::uint32_t subSequenceArray) :
                    mFullSequenceArray(fullSequenceArray), 
                    mSubSequenceArray(subSequenceArray), 
                    mMaxIndex(fullSequenceArray/subSequenceArray)
    {

    }

    std::uint32_t getMaxIndex()
    {
        return mMaxIndex;
    }

    void createSequence()
    {
        for (auto i = static_cast<std::uint32_t>(0); i < mFullSequenceArray; ++i)
        {
           fullContainer.push_back (border + std::rand() % range);
        }
    }

    void GetSequence(std::uint32_t index, T&subContainer)
    {
        for (auto i = static_cast<std::uint32_t>(index*mSubSequenceArray); i < static_cast<std::uint32_t>((index + static_cast<std::uint32_t>(1))*mSubSequenceArray); ++i)
        {
           subContainer.push_back (fullContainer.at(i));
        }
    }

    void showFullSequence()
    {
        for (auto i : fullContainer)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    void showSequence(T& container)
    {
        for (auto i : container)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

private:
    static constexpr std::uint32_t border = static_cast<std::uint32_t>(5);
    static constexpr std::uint32_t range = static_cast<std::uint32_t>(10); 

    std::uint32_t mFullSequenceArray;
    std::uint32_t mSubSequenceArray;
    std::uint32_t mMaxIndex;

    T fullContainer;
};

template<typename T>
class Task
{
public:
    Task(const T container) : mContainer(container){}

    void operator() (std::uint32_t& outgoingResult)
    {
        std::uint32_t result = static_cast<std::uint32_t>(0);
        for (auto n : mContainer)
        {
           result += n;
        }
        outgoingResult += result;
    }
    void check ()
    {
        for (auto n : mContainer)
        {
           std::cout << n;
        }
        std::cout << std::endl;
    }

private:
    T mContainer; 
};
 
int main(int argc, char* argv[])
{
    std::srand(std::time(nullptr));
    Generator<std::vector<std::uint32_t>> oGenerator(static_cast<std::uint32_t>(std::atof(argv[1])), static_cast<std::uint32_t>(std::atof(argv[2])));
    oGenerator.createSequence();
    std::cout << "Generating an array of random " << static_cast<std::uint32_t>(std::atof(argv[1])) << " numbers: ";
    //oGenerator.showFullSequence();
    std::vector<std::uint32_t>* sequenceContainer = new std::vector<std::uint32_t>[oGenerator.getMaxIndex()];
    std::cout << "Splitting the array for " << oGenerator.getMaxIndex() << " parts: " << std::endl; 
    std::vector<Task<std::vector<std::uint32_t>>> taskContainer;
    std::uint32_t* arrayOfResult = new std::uint32_t[oGenerator.getMaxIndex()];
    std::vector<std::thread> threadContainer;
    for(std::uint32_t i = static_cast<std::uint32_t>(0); i < oGenerator.getMaxIndex(); ++i)
    {
       oGenerator.GetSequence(i, sequenceContainer[i]);
       //std::cout << " * " << i + static_cast<std::uint32_t>(1) << " part is: "; 
       //oGenerator.showSequence(sequenceContainer[i]);
       taskContainer.push_back(Task<std::vector<std::uint32_t>>(sequenceContainer[i]));
       arrayOfResult[i] = static_cast<std::uint32_t>(0);
       threadContainer.push_back(std::thread(std::move(taskContainer.at(i)), std::ref(arrayOfResult[i])));
    }
    std::cout << "Waiting for completion " << oGenerator.getMaxIndex() << " threads " << std::endl;     
    
    auto start = std::chrono::high_resolution_clock::now(); 
    for(std::uint32_t i = static_cast<std::uint32_t>(0); i < oGenerator.getMaxIndex(); ++i)
    {
        if (threadContainer.at(i).joinable())
        {
           threadContainer.at(i).join();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time of " << elapsed.count() << " sec" << std::endl;
    
    std::uint32_t result = static_cast<std::uint32_t>(0);
    for(std::uint32_t i = static_cast<std::uint32_t>(0); i < oGenerator.getMaxIndex(); ++i)
    {
        //std::cout << " * " << i + static_cast<std::uint32_t>(1) << " sum is: "; 
        //std::cout << arrayOfResult[i] << std::endl;
        result += arrayOfResult[i];
    }
    std::cout << " Get full sum: ";
    std::cout << result << std::endl;
    return 0;
}