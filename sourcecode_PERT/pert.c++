// ...existing code...
#include "pert.h"
#include <iostream>
#include <queue>
#include <limits>
#include <algorithm>

// 添加活动
bool PERT::addActivity(const std::string &id,
                       float duration,
                       const std::vector<std::string> &predecessors)
{
    if (id.empty() || duration < 0.0f)
        return false;
    if (activities.count(id))
        return false;
    Activity a;
    a.activity_id = id;
    a.duration = duration;
    a.predecessors = predecessors;
    activities[id] = std::move(a);
    topo_order.clear();
    critical_path.clear();
    project_duration = 0.0f;
    return true;
}

void PERT::clear()
{
    activities.clear();
    topo_order.clear();
    critical_path.clear();
    project_duration = 0.0f;
}

const Activity *PERT::find(const std::string &id) const
{
    auto it = activities.find(id);
    return it == activities.end() ? nullptr : &it->second;
}

// 构建图: 填 successors, 校验前驱
bool PERT::buildGraph(std::string &err)
{
    for (auto &kv : activities)
        kv.second.successors.clear();

    for (auto &kv : activities)
    {
        for (auto &p : kv.second.predecessors)
        {
            if (!activities.count(p))
            {
                err = "Processor not exist: " + p + " (Activity: " + kv.first + ")";
                return false;
            }
            activities[p].successors.push_back(kv.first);
        }
    }
    return true;
}

// 拓扑排序(Kahn)
// 拓扑排序用于确保活动的执行顺序满足依赖关系（前驱活动必须先完成）
bool PERT::topoSort(std::string &err)
{
    // 1. 初始化入度表
    std::map<std::string, int> indeg; // 每个节点的入度
    for (auto &kv : activities)
    {
        indeg[kv.first] = 0; // 初始化所有节点入度为 0
    }

    // 2. 构建入度表
    for (auto &kv : activities)
    {
        for (auto &s : kv.second.successors)
        {
            indeg[s]++; // 后继节点的入度加 1
        }
    }

    // 3. 初始化队列
    std::queue<std::string> q;
    for (auto &kv : indeg)
    {
        if (kv.second == 0)
        { // 入度为 0 的节点加入队列
            q.push(kv.first);
        }
    }

    // 4. 处理队列
    topo_order.clear(); // 清空拓扑排序结果
    while (!q.empty())
    {
        auto u = q.front(); // 取出队首节点
        q.pop();
        topo_order.push_back(u); // 将节点加入拓扑排序结果

        // 遍历该节点的所有后继节点
        for (auto &s : activities[u].successors)
        {
            indeg[s]--; // 后继节点的入度减 1
            if (indeg[s] == 0)
            { // 如果入度变为 0，加入队列
                q.push(s);
            }
        }
    }

    // 5. 检测环
    if (topo_order.size() != activities.size())
    {
        err = "检测到环(循环依赖)";
        topo_order.clear(); // 清空拓扑排序结果
        return false;       // 返回失败
    }

    return true; // 拓扑排序成功
}

// Forward: ES/EF
bool PERT::forwardPass()
{
    project_duration = 0.0f;
    for (auto &id : topo_order)
    {
        Activity &act = activities[id];
        float es = 0.0f;
        for (auto &p : act.predecessors)
            es = std::max(es, activities[p].EF);
        act.ES = es;
        act.EF = act.ES + act.duration;
        project_duration = std::max(project_duration, act.EF);
    }
    return true;
}

// Backward: LS/LF
bool PERT::backwardPass()
{
    for (auto it = topo_order.rbegin(); it != topo_order.rend(); ++it)
    {
        Activity &act = activities[*it];
        if (act.successors.empty())
        {
            act.LF = project_duration;
        }
        else
        {
            float lf = std::numeric_limits<float>::infinity();
            for (auto &s : act.successors)
                lf = std::min(lf, activities[s].LS);
            act.LF = lf;
        }
        act.LS = act.LF - act.duration;
    }
    return true;
}

// Slack
void PERT::calculateSlack()
{
    for (auto &kv : activities)
    {
        kv.second.slack = kv.second.LS - kv.second.ES;
        if (kv.second.slack == -0.0f)
            kv.second.slack = 0.0f;
    }
}

// Critical Path (首条零松弛链)
void PERT::deriveCriticalPath()
{
    critical_path.clear();
    for (auto &id : topo_order)
    {
        const Activity &a = activities.at(id);
        if (a.predecessors.empty() && a.slack == 0.0f)
        {
            std::string cur = id;
            critical_path.push_back(cur);
            while (true)
            {
                const Activity &curA = activities.at(cur);
                std::string next;
                for (auto &s : curA.successors)
                {
                    const Activity &cand = activities.at(s);
                    if (cand.slack == 0.0f &&
                        curA.EF == cand.ES)
                    {
                        next = s;
                        break;
                    }
                }
                if (next.empty())
                    break;
                critical_path.push_back(next);
                cur = next;
            }
            break;
        }
    }
}

// 主计算
bool PERT::calculate()
{
    if (activities.empty())
    {
        std::cerr << "没有活动\n";
        return false;
    }
    std::string err;
    if (!buildGraph(err))
    {
        std::cerr << "构建失败: " << err << "\n";
        return false;
    }
    if (!topoSort(err))
    {
        std::cerr << "拓扑失败: " << err << "\n";
        return false;
    }
    if (!forwardPass())
        return false;
    if (!backwardPass())
        return false;
    calculateSlack();
    deriveCriticalPath();
    return true;
}

// 打印
void PERT::printActivities() const
{
    if (topo_order.empty())
    {
        std::cout << "尚未计算\n";
        return;
    }
    std::cout << "ID\tDur\tES\tEF\tLS\tLF\tSlack\tPreds\n";
    for (auto &id : topo_order)
    {
        const auto &a = activities.at(id);
        std::cout << a.activity_id << '\t'
                  << a.duration << '\t'
                  << a.ES << '\t'
                  << a.EF << '\t'
                  << a.LS << '\t'
                  << a.LF << '\t'
                  << a.slack << '\t';
        for (size_t i = 0; i < a.predecessors.size(); ++i)
        {
            if (i)
                std::cout << ",";
            std::cout << a.predecessors[i];
        }
        std::cout << "\n";
    }
    std::cout << "Project Duration: " << project_duration << "\n";
    if (!critical_path.empty())
    {
        std::cout << "Critical Path: ";
        for (size_t i = 0; i < critical_path.size(); ++i)
        {
            if (i)
                std::cout << " -> ";
            std::cout << critical_path[i];
        }
        std::cout << "\n";
    }
}

// 关键路径访问
std::optional<std::vector<std::string>> PERT::getCriticalPath() const
{
    if (critical_path.empty())
        return std::nullopt;
    return critical_path;
}
// ...existing code...
