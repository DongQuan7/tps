
#ifndef TPS_DA_MACHINGTABLES_H_
#define TPS_DA_MACHINGTABLES_H_


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>



TPS_BEGIN_NAMESPACE
template<class Class, typename Query>
class RtTableBase
{
private:
    typedef boost::multi_index::multi_index_container<Class, Query> MultiIndexContainer;
    MultiIndexContainer mCon;

    struct Change_Item
    {
        Change_Item(const Class& item1){
            item = item1;
        }

        void operator()(Class& e)
        {
            e = item;
        }

    private:
        Class item;
    };

public:
    RtTableBase()
    {
    }

    ~RtTableBase()
    {
        while (!mCon.empty())
        {
            typename MultiIndexContainer::iterator it = mCon.begin();
            mCon.erase(it);
        }
    }

    template<typename Tag, typename Tuple>
    int Get(const Tuple& tuple, Class& result)
    {
        typedef MultiIndexContainer::index<Tag>::type MultiIndexContainerType;
        MultiIndexContainerType& i = boost::multi_index::get<Tag>(mCon);
        auto it = i.find(tuple);
        if (it != i.end())
        {
            result = *it;
            return 1;
        }
        else
        {
            return 0;
        }
    }

    template<typename Tag, typename Tuple>
    int Get(const Tuple& tuple, std::shared_ptr<Class>& result)
    {
        typedef MultiIndexContainer::index<Tag>::type MultiIndexContainerType;
        MultiIndexContainerType& i = boost::multi_index::get<Tag>(mCon);
        auto it = i.find(tuple);
        if (it != i.end())
        {
            result.reset(new Class(*it));
            return 1;
        }
        else
        {
            result = nullptr;
            return 0;
        }
    }

    template<typename Tag, typename Tuple>
    int Get(const Tuple& tuple, std::vector<std::shared_ptr<Class>>& result)
    {
        typedef MultiIndexContainer::index<Tag>::type::iterator MultiIndexContainerIterator;
        typedef MultiIndexContainer::index<Tag>::type MultiIndexContainerType;

        MultiIndexContainerType& i = boost::multi_index::get<Tag>(mCon);
        MultiIndexContainerIterator it0, it1;
        boost::tie(it0, it1) = i.equal_range(tuple);

        int count = 0;
        if (it0 == it1)
        {
            return count;
        }
        while (it0 != it1)
        {
            std::shared_ptr<Class> ptr(new Class(*it0));
            result.push_back(ptr);
            ++it0;
            ++count;
        }
        return count;
    }

    template<typename Tag, typename Tuple>
    int Get(const Tuple& tuple, std::vector<Class>& result)
    {
        typedef MultiIndexContainer::index<Tag>::type::iterator MultiIndexContainerIterator;
        typedef MultiIndexContainer::index<Tag>::type MultiIndexContainerType;

        MultiIndexContainerType& i = boost::multi_index::get<Tag>(mCon);
        MultiIndexContainerIterator it0, it1;
        boost::tie(it0, it1) = i.equal_range(tuple);

        int count = 0;
        if (it0 == it1)
        {
            return count;
        }
        while (it0 != it1)
        {
            result.push_back(*it0);
            ++it0;
            ++count;
        }
        return count;
    }

    int GetAll(std::vector<Class>& result)
    {
        int count = 0;
        for (auto it = mCon.begin(); it != mCon.end(); it++)
        {
            result.push_back(*it);
            ++count;
        }
        return count;
    }

    template<typename Tag, typename Tuple>
    int Delete(const Tuple& tuple)
    {
        typedef MultiIndexContainer::index<Tag>::type::iterator MultiIndexContainerIterator;
        typedef MultiIndexContainer::index<Tag>::type MultiIndexContainerType;

        MultiIndexContainerType& i = boost::multi_index::get<Tag>(mCon);
        MultiIndexContainerIterator itStart, itEnd;

        boost::tie(itStart, itEnd) = i.equal_range(tuple);

        int count = 0;
        if (itStart == itEnd)
        {
            return count;
        }

        while (itStart != itEnd){
            itStart = i.erase(itStart);
            ++count;
        }
        return count;
    }

    int DeleteAll()
    {
        int count = 0;
        auto itStart = mCon.begin();
        while (itStart != mCon.end()){
            itStart = mCon.erase(itStart);
            ++count;
        }
        return count;
    }

    bool Add(const Class& item)
    {
        return mCon.insert(item).second;
    }

    template<typename Tag, typename Tuple>
    bool Update(const Tuple& tuple, const Class& item)
    {
        typedef MultiIndexContainer::index<Tag>::type::iterator MultiIndexContainerIterator;
        typedef MultiIndexContainer::index<Tag>::type MultiIndexContainerType;

        MultiIndexContainerType& i = boost::multi_index::get<Tag>(mCon);
        auto it = i.find(tuple);
        if (it != i.end())
        {
            Class oldValue = *it;
            bool result = mCon.modify(it, Change_Item(item), Change_Item(oldValue));
            return result;
        }
        else
        {
            return false;
        }
    }
};

TPS_END_NAMESPACE
#endif