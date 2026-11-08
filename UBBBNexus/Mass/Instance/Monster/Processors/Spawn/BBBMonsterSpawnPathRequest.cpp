#include "BBBMonsterSpawnPathRequest.h"
#include "NavigationSystem.h"
#include "Async/Async.h"

void UBBBMonsterSpawnPathRequest::Start(UNavigationSystemV1 &Navigation, const ANavigationData &NavData)
{
    bComplete = false;
    Path.Reset();
    NavigationData = &NavData;
    while (Observers.IsValidIndex(ObserverIndex))
    {
        FNavLocation Projected;
        if (!Navigation.ProjectPointToNavigation(Observers[ObserverIndex], Projected, FVector(150, 150, 600), &NavData))
        {
            ++ObserverIndex;
            continue;
        }
        Goal = Projected.Location;
        FPathFindingQuery Query(nullptr, NavData, Location, Goal, NavData.GetDefaultQueryFilter());
        Query.SetAllowPartialPaths(false);
        QueryId = Navigation.FindPathAsync(NavData.GetConfig(), Query,
            FNavPathQueryDelegate::CreateWeakLambda(this,
                [Weak = TWeakObjectPtr<UBBBMonsterSpawnPathRequest>(this)]
                (uint32, ENavigationQueryResult::Type QueryResult, FNavPathSharedPtr QueryPath)
                {
                    auto Complete = [Weak, QueryResult, QueryPath = MoveTemp(QueryPath)]() mutable
                    {
                        if (auto *Request = Weak.Get())
                        {
                            Request->Result = QueryResult;
                            Request->Path = MoveTemp(QueryPath);
                            Request->bComplete = true;
                        }
                    };
                    if (!IsInGameThread())
                    {
                        AsyncTask(ENamedThreads::GameThread, MoveTemp(Complete));
                        return;
                    }
                    Complete();
                }));
        if (QueryId == INVALID_NAVQUERYID)
        {
            Result = ENavigationQueryResult::Error;
            bComplete = true;
        }
        return;
    }
    Result = ENavigationQueryResult::Fail;
    QueryId = INVALID_NAVQUERYID;
    bComplete = true;
}
