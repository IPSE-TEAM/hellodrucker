# 德鲁克计划

### 合约账号 hellodrucker

为了致敬管理学大师彼得·德鲁克，合约取名```hellodrucker```。

团队内部如何调动成员积极性，如何激励团队和发挥成员潜能，如何让团队更加凝聚发挥整体的效能，这些一直都是热门话题。

工业革命以来的企业组织管理，KPI发源于工厂，作为一套完整的考核制度，在过去都是一个不错的管理工具，不过到了新时代，所有管理者都会遇到一个问题：现在的90后，95后和未来的00后，这些互联网时代原住民，KPI考核制度并不是那么完美，新时代的年轻员工需要更多的自主权，希望能在自己能支配的范围内发挥自己的自主权。

随着互联网发展到了价值网络的层次，数位经济无处不在，团队激励制度也需要相应的更新。```Hellodrucker```就是为这样的更新所做的尝试。深入分析了年轻人组成的团队，结合现在的区块链技术，发明了全新的团队Token激励制度。

任何公司或者团队组织者都可以定下一批奖励金Token，规则也非常简单，团队成员每个人在月初都能分到均等的Token，每个团队成员（个人或者小组）都需要在接下来一个月将账户上的Token送给队友，不管是何种理由，还是何种数额，如果没有完全送出去，就需要扣罚掉没有送出去的余额。根据收到Token的数额排序，奖励最高的前3名。除了把扣罚的奖励给前3名外，发起者还可以设置额外的奖励，比如第一名奖励Token总数的30%，第二名奖励Token总数的20%，第三名奖励Token总数的10%。

### 技术细节

- ```sponsor``` 发起人表

		address：发起人地址
		sponsorname：发起人名字
		totaltoken:总投入Token数量
		balance：余额
		members:团队成员总数
		incentive：每个成员激励数量
		slashtoken：扣罚Token数量
		timestamp：启动激励计划时的时间戳
		period：激励计划的周期，比如一个月
		phase：第多少期，比如第1期。
		status：激励计划状态 registered，starting，settling
		
> 方法1： 注册发起人typein
> 
> 方法2： 发起人注入Token
> 
> 方法3： 发起人启动激励计划launch
> 
> 方法4:  发起人结算settle，启动结算后，团队成员每个人都可以提币。同时发起人可以扣罚掉没有送出去的Token。
>  
> 方法5:  发起人扣罚掉成员未送出的Token，slash。当状态为settling，可以扣罚掉团队成员的outbalance。
> 
> 方法6:  注销团队成员cancelmember
> 
> 方法7:  注销发起人cancelsponsor,如果balance为0，则合约账号可以删掉这条数据，降低内存消耗。
> 
> 方法8:  发起人提取余额withdrawbal,可提取金额，不能高过balance-members*incentive.
> 
> 方法9:  发起人接受团队成员加入accept，如果sponsor是registered状态和starting状态，member可以接收到token，sponsor是setting状态，member不接收token。

- ```teammember``` 团队成员表

		address：成员地址
		sponsoraddr：发起人地址
		membername：成员名字
		in：收到的Token数量
		out：累计发送出去的Token数量
		outbalance: 发送出去的Token余额
		incentive：每个成员激励数量
		balance：余额
		phase：第多少期，比如第1期
		status：joining，joined，cancelled
		
> 方法1：成员加入join，发起人表状态为registered，则可在下一轮中得到激励Token，如果状态为starting和settling，则需要在下一轮才能得到激励Token。
> 
> 方法2: 成员离开leave,如果没有提出的Token，则直接删除掉这行数据。合约账号可以删掉这行cancelled数据，为了降低内存消耗。
>
> 方法3: 成员Token发送send，当phase跟发起人表的phase相等时，可以顺利发送。如果phase小于发起人表的phase，则说明在结算期，成员没有提取Token，将in中Token转移到balance，out替换成上一个incentive的数量，更新phase跟sponsor表匹配上。
> 
> 方法4: Token提取withdraw，当发起人表状态为settling时，当phase小于发起人表的phase，可提取balance，相等时，可以提取in+balance。
> 
> 方法5: 合约账号删除成员delmember，只要成员的状态为cancelled，即可删除。
> 
> 如果团队成员错过好几期，当然发起人可以扣罚掉没有发送出的outbalance，如果没有扣罚，而成员有没有提取属于自己的in数额，那只能让合约账号获得这部分利益。

### 盈利

发起人注入Token的时候，收取5%的手续费，用于合约内存消耗的成本。

		
		
	
