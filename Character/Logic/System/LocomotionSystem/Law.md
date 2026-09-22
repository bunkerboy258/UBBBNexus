# Locomotion System Law

- 本系统只把解析后的控制事实应用到角色与 CMC，并形成最终移动状态。
- 所有 CMC 写入必须集中在 Locomotion Processor，禁止其它领域直接产生移动输入。
