# BBBWork 更新日志

## 2026-11-08
- [UBBBNexus] Align source layout with repository law
## 2026-11-05
- [UBBBNexus] Prevent life updates from re-enabling damage during replay playback
- [UBBBNexus] Record standalone Mass facts and release replay presentation pools safely
- [UBBBNexus] Add V01 tutorial bridge and Maodie grenade drops
- [UBBBNexus] Fix gameplay cursor and backpack mouse focus
- [UBBBNexus] fix(animation): smooth fall ragdoll recovery
- [UBBBNexus] Keep locomotion direction curve alive across world garbage collection
- [UBBBNexus] Fix automatic pickup reach and play successful pickup feedback
- [UBBBNexus] Fix local camera ownership for network possession
- [UBBBNexus] Make Maodie face and hiss at the player who shot it
- [UBBBNexus] Detect player capsules during melee sweeps
- [UBBBNexus] Keep the local player camera and scale monster waves
- [UBBBNexus] fix(animation): wait for get-up blend-out before restoring movement
## 2026-11-04
- [UBBBNexus] Add orange character lighting and visible inner outline for Maodie
- [UBBBNexus] 增加本地角色专用补光与可见内侧轮廓
- [UBBBNexus] Add local aim visualization with correlated projectile launch and impact records
- [UBBBNexus] Fix wandering monsters choosing paths toward an empty chase target
- [UBBBNexus] Use V3 texture expression and per-player randomized Maodie drops
- [UBBBNexus] Prevent character ragdoll from blocking camera probes
- [UBBBNexus] Limit aim debug display to live muzzle ray and two aim points
- [UBBBNexus] Separate zombie patrol and pursuit animation timing
- [UBBBNexus] Unify quality application and remove redundant monster movement work
- [UBBBNexus] Add per-player Maodie following and independent head tracking
- [UBBBNexus] Fix monster processor include dependencies for editor build
- [UBBBNexus] Show airborne world drops using a visible parcel attached to physics
- [UBBBNexus] Preserve shared skin tone when a mirror becomes locally controlled
- [UBBBNexus] Respect zombie source animation cadence and per-action attack contact times
- [UBBBNexus] Add Maodie exploration counter and Evac completion HUD
- [UBBBNexus] Reduce first-use combat and inventory presentation stalls
- [UBBBNexus] Enforce a 100 meter cap for center-screen aim target queries
- [UBBBNexus] Fix backpack portrait refresh across every visible tab
- [UBBBNexus] Keep world drop meshes visible until pickup and show airborne markers
- [UBBBNexus] Initialize shared natural skin tone for base head and arms
## 2026-11-03
- [UBBBNexus] 补齐消耗品换弹生命反馈与CH001常驻准星
- [UBBBNexus] Unify target hit responses and configure body regions and stagger strength
- [UBBBNexus] Add shared world item drops and character auto pickup
- [UBBBNexus] Refine world drop beam icon and ground glow presentation
- [UBBBNexus] 修复背包缺失贴图导致的白色覆盖层
- [UBBBNexus] Add guaranteed melee stagger and player knockdown contributions
- [UBBBNexus] Add configurable Maodie wander hiss and limited world drop test actor
- [Constitution] Migrate hit strength and target body configuration assets
- [UBBBNexus] Add presentation-only state notify boundary for GASP Foley
## 2026-11-02
- [Constitution] Align hit contract and editor validation with unified combat inputs
- [UBBBNexus] Preserve physical impact payload when queueing character damage
- [UBBBNexus] Polish slime presentation with pooled liquid effects and fading puddles
- [UBBBNexus] Remove unused slime source-template parameter writes
- [UBBBNexus] Restore gravity and release animation drives during character ragdoll
- [UBBBNexus] Polish special infected action phases and physical hit presentation
- [UBBBNexus] Remove small flesh debris from zombie hit presentation
- [UBBBNexus] Polish throwable trails and authoritative collision audio
- [UBBBNexus] Unify combat hits as target-owned additive contributions
- [UBBBNexus] Fix equipment message ordering and unarmed montage handoff
- [UBBBNexus] Request reload through weapon input when firing empty
- [UBBBNexus] Add airborne fall ragdoll and single landing damage settlement
- [UBBBNexus] Add configurable individual curves to zombie travel before avoidance
## 2026-11-01
- [Constitution] Clarify animation ownership and simplify character update graphs
- [UBBBNexus] refactor(animation): publish movement facts and derive recoil in animation updates
- [UBBBNexus] Fix zombie steering stability and melee approach retention
- [UBBBNexus] Keep generic weapon animation calculations in dedicated processors
- [UBBBNexus] Replace zombie navigation with Detour corridors and velocity avoidance
- [UBBBNexus] fix(ui): preview first equipment when opening backpack
- [UBBBNexus] Guard zombie preview residue cleanup when editor world has no presentation pools
- [UBBBNexus] Add character ragdoll entry through the input pipeline
- [UBBBNexus] Implement notify-driven ragdoll recovery with GASP pose selection
- [UBBBNexus] Implement special infected skills and player combat impacts
- [UBBBNexus] Add Mass zombie mantle and climb navigation with bounded movement
## 2026-10-31
- [UBBBNexus] Keep zombie hit reactions aligned with animated joints and continuous bursts
- [UBBBNexus] Replace transient flesh particles with pooled physical residue
- [UBBBNexus] Smooth camera stance height independently of character movement
- [UBBBNexus] Allow corpse damage and progressive prefab dismemberment
- [UBBBNexus] Fix zombie crowd progress, attack approach slots and runtime diagnostics
## 2026-10-30
- [UBBBNexus] Allow character recoil animation weights up to two
- [UBBBNexus] Allow randomized equipment object ejection direction
## 2026-10-29
- [UBBBNexus] docs(locomotion): explain critical GASP strafe facing bug fix
## 2026-10-28
- [UBBBNexus] Expose projectile presentation to editor preview
- [UBBBNexus] fix(animation): publish wall blocking facts
- [UBBBNexus] Limit character facing rotation to 360 degrees per second
- [UBBBNexus] fix(locomotion): preserve abrupt strafe facing for motion matching
## 2026-10-27
- [UBBBNexus] Use continuous weapon recoil curves
- [UBBBNexus] Add batched zombie flesh and wound hit presentation
- [UBBBNexus] 完善僵尸路线质量与出生回收边界
- [UBBBNexus] 核验毁伤运行网格预算与腔壁烘焙标记
- [UBBBNexus] 正确初始化出生复核查询与实际脚底高度
## 2026-10-26
- [UBBBNexus] Drive rescue completion through montage notify input
- [UBBBNexus] 实现僵尸八区持久毁伤与同级断肢分区
- [UBBBNexus] Retire character traversal and expose GASP ground recovery facts
- [UBBBNexus] feat(animation): publish GASP facts and simplify traversal montage handoff
## 2026-10-25
- [UBBBNexus] Publish movement snapshots for Motion Matching trajectories
- [UBBBNexus] revert: remove free arm damage experiment and retain unified gore
- [UBBBNexus] feat(debug): add tested local arm damage accumulation and connectivity core
## 2026-10-24
- [UBBBNexus] Unify monster logic and presentation configuration
- [Constitution] Normalize Mass law line endings
- [Constitution] Restore complete Mass physical directory contract
- [Constitution] Condense Mass law to shared architecture rules
- [UBBBNexus] Polish weapon firing responsiveness and recoil contributions
- [UBBBNexus] Replace zombie gore with bounded unified wound presentation
- [UBBBNexus] docs: normalize UBBBNexus Law files
- [UBBBNexus] Polish prefab wound selection and remove editor-only preview state
- [UBBBNexus] Update item resource paths to formal item directories
- [UBBBNexus] Configure zombie debug targets from production versions with explicit overrides
- [UBBBNexus] Document monster and projectile architecture in dedicated laws
- [UBBBNexus] Explain monster and projectile architecture and lifecycle with diagrams
- [UBBBNexus] Map monster and projectile directories to plain Chinese architecture terms
- [UBBBNexus] Format monster and projectile physical layouts as annotated directory trees
- [UBBBNexus] Match detached limb poses by bone name
- [Constitution] docs: normalize moved Constitution Law files
## 2026-10-23
- [UBBBNexus] Replace impulse recoil with cumulative shot patterns and immediate recovery
- [UBBBNexus] Drive recoil through animation pose facts instead of aim target offsets
- [UBBBNexus] Calibrate zombie locomotion phase and expose presentation yaw facts
- [UBBBNexus] 启用腰部挂件受限物理摆动并验证显示生命周期
- [UBBBNexus] Drive equipment visibility through character animation inputs
- [UBBBNexus] 稳定背包人物预览姿态与表面捕获
## 2026-10-22
- [UBBBNexus] Prebuild debug zombie gore and bound repeated hit rendering work
- [UBBBNexus] Drive dual character grip poses from existing fire intent
## 2026-10-21
- [UBBBNexus] Unify monster spawning with bounded deterministic Mass requests
## 2026-10-20
- [UBBBNexus] Fix runtime build use of constraint profile and material slots
## 2026-10-19
- [UBBBNexus] Restore complete FABRIK weapon grip facts and wrist calibration
- [UBBBNexus] Move hand wrist orientation to grip animation and keep IK position only
- [UBBBNexus] Integrate APS navigation with bounded Mass zombie replanning
- [UBBBNexus] Use wet blood red defaults for isolated zombie cut surfaces
- [UBBBNexus] Expose main animation curves for self-contained linked hand layers
- [UBBBNexus] Rebuild isolated zombie target wounds and multi-segment severing
- [UBBBNexus] Publish per-weapon right wrist grip facts and canonical portrait animation
## 2026-10-18
- [UBBBNexus] Enable consistent left-hand IK targets for pistols
- [UBBBNexus] Organize equipment configuration categories and field display order
- [UBBBNexus] Add equipment hand grip pose animation snapshot
- [UBBBNexus] Fix single-cycle shotgun reload completion
- [UBBBNexus] Add configurable zombie gore debug target with periodic reset
## 2026-10-17
- [UBBBNexus] Keep equipment visible while using throwables
- [UBBBNexus] Replace staged grenade throws with layer-selected Lyra quick toss
- [UBBBNexus] Implement character-owned consumable use and quick melee
## 2026-10-16
- [Constitution] Align consumable laws with use-only runtime and character synchronization
- [Constitution] Track verified project logic assets without art content
- [UBBBNexus] Unify Actor input contribution and traversal item visibility
- [UBBBNexus] fix(monster): restore procedural hit reaction profile loading
- [UBBBNexus] Add isolated stationary zombie hit reaction tuning target
- [UBBBNexus] Fix zombie tuning target ignoring projectile trace channel
- [UBBBNexus] Tune debug zombie hit response with measured bone trajectories
- [UBBBNexus] Define consumables as use-only objects with notification-driven actions
- [UBBBNexus] 持枪时常驻右下弹药数字并保留瞄准短弧
- [UBBBNexus] 枪口落点检测Mass实体并回归验证常驻弹药HUD
## 2026-10-15
- [UBBBNexus] 完善手雷贴墙释放与正式物理验收
- [UBBBNexus] Implement staged shotgun per-shell reload and firing interruption
- [UBBBNexus] Reduce ammo rail radius and double stroke width
- [UBBBNexus] Add host-authoritative Chaos throwables with predictive interpolation
- [UBBBNexus] 将背包主界面改为中文距离场艺术字
- [UBBBNexus] Remove duplicate consumable HUD strip and scope use hint to held items
- [UBBBNexus] Verify nonmetallic zombie wound and blood surfaces
- [UBBBNexus] Preserve reticle ammo arc independently of corner HUD layout
- [UBBBNexus] 独立背包武器角色消耗品导航并调整中文排版
- [UBBBNexus] 调整背包导航文字与选中标记间距
- [UBBBNexus] Remove obsolete consumable strip declaration comment
## 2026-10-14
- [UBBBNexus] Verify default game mode player input bindings
- [UBBBNexus] Fix inventory portrait modular pose alignment
- [UBBBNexus] Refactor item input contracts, consumable lifecycle, and system contexts
- [UBBBNexus] 明确物品基类结构与具体实例职责规范
- [UBBBNexus] 清除武器侧角色蒙太奇停止链路并恢复槽位覆盖模型
- [UBBBNexus] 修正爆炸自伤归属并验证原生角色伤害
- [UBBBNexus] 恢复换弹期间切枪并分离快捷消耗品权限
- [UBBBNexus] 重组物品分类并接入独立消耗品交互
## 2026-10-13
- [UBBBNexus] feat(mass): add configurable dungeon monster director
- [UBBBNexus] feat(mass): rebuild projectile-driven body stagger reactions
## 2026-10-12
- [UBBBNexus] Validate zombie gib materials against source polygon groups
- [UBBBNexus] Allow walkers to run on close pursuit with fixed birth distance
- [UBBBNexus] Add crouched weapon modifiers and clarify editor configuration
- [UBBBNexus] Validate zombie gib geometry and respect skeletal physics lifetime
- [UBBBNexus] Tune camera recoil pitch retention for measured reference behavior
- [UBBBNexus] Allow budgeted zombie gib physics on close LOD2 bodies
## 2026-10-11
- [UBBBNexus] Stabilize sustained recoil and delay post-shot recovery
- [UBBBNexus] Repair equipment display velocity and held-object cleanup
- [UBBBNexus] Register trousers and misc belts in wearable slots
- [UBBBNexus] Fix zombie skull hits and converge physical aim to near contacts
- [UBBBNexus] Tune third-person camera composition for hip and aim
- [UBBBNexus] Use model-root melee definition path and explicit package include
- [UBBBNexus] Restore continuous recoil recovery to bound automatic fire climb
- [UBBBNexus] Rebuild pooled zombie severing and shallow body wounds
- [UBBBNexus] Give severed zombie parts independent flight and real ground contact
## 2026-10-10
- [UBBBNexus] Update item asset references after character inventory organization
- [UBBBNexus] Reduce zombie collision publication and stationary ground sweeps
- [UBBBNexus] Reduce zombie collision bucket duplication and stationary overlap costs
- [UBBBNexus] Complete firearm cadence, minigun spin and per-round reload presentation
- [UBBBNexus] Allow handgun configurations without left-hand IK sockets
- [UBBBNexus] Rebuild zombie part damage, physical reactions, severing and combat presentation
## 2026-10-09
- [UBBBNexus] Rebuild zombie perception and bounded navigation decisions
- [UBBBNexus] Drive authored zombie stagger and visible hit stop from Mass facts
- [UBBBNexus] Rebuild inventory UI from E01 source art and capture appearance in player world
- [UBBBNexus] Polish E01 inventory readability and restore usable storage scrolling
- [UBBBNexus] Unify E01 text as faded white lettering and fill viewport backdrop
- [UBBBNexus] Enlarge Bag heading and layer item names over expanded previews
- [UBBBNexus] Clean white UI typography and refine enlarged item preview layout
- [UBBBNexus] Replace equipment debug spawner with unified item injector
- [UBBBNexus] Finish English inventory feedback and provide scoped UI cleanup script
- [UBBBNexus] Save cleanup script with UTF-8 BOM for Windows PowerShell
- [UBBBNexus] Restrict quickbar to weapons and equip inventory items on double click
- [UBBBNexus] Separate concurrent appearance changes from physical animation fix
- [UBBBNexus] Build deterministic zombie birth variation and asynchronous crowd animation
- [UBBBNexus] Read character muzzle facts from equipment capability
- [UBBBNexus] Add concrete ModernWeapons firearm types and shotgun per-round loading
- [UBBBNexus] Apply appearance entirely in C++ and add rigid wearable attachment
- [UBBBNexus] Expose generic equipment animation snapshots for gun layers
- [UBBBNexus] Prevent physical animation and parallel bone evaluation deadlock
- [UBBBNexus] Fix melee validation asset path after config organization
- [UBBBNexus] Remove deferred unequip messages and equipment switch tick ordering
- [UBBBNexus] Align zombie step navigation and add budgeted retained corpses
- [UBBBNexus] Give each rifle its own character recoil animation snapshot
- [UBBBNexus] Add shared mesh explosive and ballistic projectile behavior
- [UBBBNexus] Split character update into explicit mirror and causal paths
- [UBBBNexus] Clarify character update pipeline stages
- [UBBBNexus] Keep ammo arc short and offset from reticle
- [UBBBNexus] Make appearance display transactional and remove stale contracts
- [UBBBNexus] Route melee attacks through upper-body montage inputs
- [UBBBNexus] Refine combat HUD ammo display
- [UBBBNexus] Allow projectile launch direction to follow firing configuration
- [UBBBNexus] Preserve bounded vertical velocity on airborne traversal handoff
## 2026-10-08
- [UBBBNexus] Move downed animation entry into inherited locomotion state
- [UBBBNexus] Use UI-only input while the backpack is open
- [UBBBNexus] Add fact-driven zombie voice presentation with global audio budgets
- [UBBBNexus] Rebuild P08 bag and loadout UI on the unified item inventory
- [UBBBNexus] Read current inventory slots in player controller queries
- [UBBBNexus] Smooth traversal handoff and reject stale movement corrections
- [UBBBNexus] Fix zombie attack eligibility, swing direction and obstruction
- [UBBBNexus] Complete character-owned cooperative rescue and inherited helping animation facts
- [UBBBNexus] Drive traversal poses through inherited states and synchronize movement facts
- [UBBBNexus] Keep weapon icon slot dimensions fixed after texture binding
- [UBBBNexus] Implement minimal player HUD and three-slot backpack controls
- [UBBBNexus] Rebuild zombie blood residue with bounded ballistic surface pooling
## 2026-10-07
- [UBBBNexus] Define inherited network foundation with equipment-specific protocols
- [UBBBNexus] Refine zombie physical impacts and sustained hit slowing
- [UBBBNexus] Rebuild equipment lifecycle and per-type input network architecture
- [UBBBNexus] Improve traversal thresholds pace and control handoff
- [UBBBNexus] Inject configured equipment list into player backpack at debug startup
- [UBBBNexus] Gate character aim IK by rifle and valid configured muzzle
- [UBBBNexus] Extract traversal system and stabilize animation movement handoff
- [UBBBNexus] Define concrete equipment directory layout and implementation rules
- [UBBBNexus] Implement character life downed movement equipment stow and physical death
- [UBBBNexus] Rebuild zombie mobility with cumulative leg damage and constrained navigation
- [UBBBNexus] Add decoupled player backpack UI and tactical equipment HUD
- [UBBBNexus] Stabilize zombie close-contact facing and attack restart
- [UBBBNexus] Define equipment base layout and instance-owned networking
- [UBBBNexus] Consolidate equipment base rules into directory entries
- [UBBBNexus] Require final equipment implementations for base lifecycle contracts
- [UBBBNexus] Separate equipment implementation requirements from directory layout
- [UBBBNexus] Shorten equipment final inheritance rules
- [UBBBNexus] Remove legacy monster hurt behavior and rebuild six-state presentation contract
- [UBBBNexus] Keep locomotion live through traversal exit and equipment recovery
## 2026-10-06
- [UBBBNexus] Add regional zombie hit reactions and shared blood feedback
- [UBBBNexus] Keep zombie chase speed and resume movement after each stationary attack
- [UBBBNexus] Implement authoritative monster gravity and ground support
- [UBBBNexus] Remove rifle definition fire sound code
- [UBBBNexus] Move character inventory and shortcut selection into ItemSystem
- [UBBBNexus] Replace ordinary zombie bone offsets with procedural physical hit reactions
- [UBBBNexus] Integrate configurable vault and climb through character pipeline and animation layers
- [UBBBNexus] Double zombie procedural hit reaction impulses
- [UBBBNexus] Fix zombie physical hit reactions at active LOD and validate projectile path
- [UBBBNexus] Rebuild zombie locomotion with random patrol and alert behavior
## 2026-10-05
- [UBBBNexus] feat(mass): drive zombie animation from facts with budgeted presentation and validation
- [UBBBNexus] localize editor metadata and normalize human-facing punctuation
- [UBBBNexus] Validate presentation smoothing with current fact animation class
- [UBBBNexus] Restrict editor automation tests to editor builds
- [UBBBNexus] Add batched projectile impact feedback and terminal tracer fade
- [UBBBNexus] Fix customization fullscreen and smooth slot previews with async loading
- [UBBBNexus] Refine customization inventory cards and studio presentation
- [UBBBNexus] Fix ejected equipment objects inheriting holder velocity
- [UBBBNexus] Add equipment fire gate inputs and animation notifies
- [UBBBNexus] Polish customization cards, pictographic controls and inward panel composition
- [UBBBNexus] Register isolated animation test world context
- [UBBBNexus] normalize Law punctuation and include approved Law revisions
- [UBBBNexus] Fix Mass hit accumulation and merge per-player cumulative damage
## 2026-10-04
- [UBBBNexus] Update project asset paths after directory migration
- [UBBBNexus] Drive Mass monster animations through explicit-time crossfade snapshots
- [UBBBNexus] Configure left-hand grip rotation and simplify animation getters
- [UBBBNexus] Add comment constitution and naming guidance
## 2026-10-03
- [UBBBNexus] Smooth client Mass monster presentation without changing gameplay state
- [UBBBNexus] Add thread-safe left-hand grip rotation facts
- [UBBBNexus] Add equipment animation display spawn notifies
## 2026-10-02
- [UBBBNexus] Separate rifle handling and camera recoil profiles with animation snapshots
- [UBBBNexus] Fit magazine release velocity from the full notify trajectory
- [UBBBNexus] Fix camera follow before player view cache update
- [UBBBNexus] Fix recoil snapshot boundaries and camera consumption ordering
## 2026-10-01
- [UBBBNexus] Configure magazine throw velocities on animation notify
- [UBBBNexus] Enable editor selection and transform editing for hand magazines
- [UBBBNexus] Preserve pre-release magazine motion when notify ends
## 2026-09-30
- [UBBBNexus] chore: snapshot existing local workspace changes at reload task completion
## 2026-09-29
- [UBBBNexus] Fix character magazine release velocity from animated world motion
- [UBBBNexus] Rebuild rifle reload notifies by target and domain
- [UBBBNexus] fix: update Mass projectile tracers after current-frame weapon animation
- [UBBBNexus] refactor: polish customization studio and simplify view composition
## 2026-09-28
- [UBBBNexus] Remove redundant rifle left-hand socket offset
- [UBBBNexus] feat: sync rifle magazine visuals with character reload
- [UBBBNexus] Move rifle magazine visuals into reload notify states
- [UBBBNexus] fix: align rifle reload magazine with left hand
- [UBBBNexus] fix: refine customization preview layout
- [UBBBNexus] feat: rebuild full-screen character customization UI
- [UBBBNexus] fix: clamp projectile tracer to traveled distance
## 2026-09-27
- [UBBBNexus] fix(input): keep firing separate from precision aim
- [UBBBNexus] Keep Mass projectile tracer particles moving with projectiles
- [UBBBNexus] refactor(mass): rebuild projectile and monster ECS runtime
- [UBBBNexus] docs(aim): remove outdated aim system comments
- [UBBBNexus] refactor(animation): derive aim IK weight in animation blueprint
- [UBBBNexus] docs: establish Mass architecture constitution
- [UBBBNexus] chore: snapshot existing workspace changes
- [UBBBNexus] Add local Mass projectile implementation
- [UBBBNexus] Refine customization preview and layout
## 2026-09-26
- [UBBBNexus] Align equipment actor boundaries and config layout
- [UBBBNexus] Restore walk and run speed tiers
- [UBBBNexus] Add independent body and vest patch selection
- [UBBBNexus] Add camera roll and character aim impulses for rifle fire
- [UBBBNexus] Refine customization preview studio lighting
- [UBBBNexus] Document character config properties
- [UBBBNexus] Add rifle debug tracer for fire cadence
- [UBBBNexus] Remove temporary rifle debug tracer
- [UBBBNexus] Normalize punctuation in UBBBNexus Law files
- [UBBBNexus] Move muzzle bone-space snapshot into character animation system
- [UBBBNexus] Add rifle fire cadence diagnostics
## 2026-09-25
- [UBBBNexus] Clarify actor architecture and network role laws
- [UBBBNexus] Improve actor constitution formatting
- [UBBBNexus] Reference actor constitution from rifle law
- [UBBBNexus] 修复角色与步枪系统职责边界
- [UBBBNexus] Align actor input packets with constitution
- [UBBBNexus] Define actor input packet constitution
- [UBBBNexus] Unify rifle input through templated actor entry
- [UBBBNexus] Update actor equipment and notify laws
- [UBBBNexus] Clarify polymorphic actor input and simplify equipment requests
- [UBBBNexus] Rebuild local character customization preview
- [UBBBNexus] Conform equipment input and lifecycle to actor law
- [UBBBNexus] Remove obsolete equipment constitution reference
## 2026-09-24
- [UBBBNexus] docs: add concise network architecture constitution
- [UBBBNexus] refactor: simplify character and equipment network architecture
- [UBBBNexus] Organize character folders and update rules
- [UBBBNexus] Document Character directory responsibilities
- [UBBBNexus] Document rifle notify input boundaries
- [UBBBNexus] Revise actor and equipment architecture laws
- [UBBBNexus] Refine character system laws
- [UBBBNexus] Simplify character core law
- [UBBBNexus] Refactor character animation and equipment update flow
- [UBBBNexus] Specify mandatory Actor structure and lifecycle rules
- [UBBBNexus] Refactor rifle input packets through action state
- [UBBBNexus] Add shared Actor constitution
- [UBBBNexus] Clarify equipment architecture alignment
- [UBBBNexus] Simplify character locomotion to walk and run
- [UBBBNexus] Add directory duties to character laws
- [UBBBNexus] Move rifle notifies into root classification
- [UBBBNexus] Document network state observation flow
## 2026-09-23
- [UBBBNexus] Move aim alpha ownership to character aim system
- [UBBBNexus] Make equipment actors own definitions and unify character item slots
- [UBBBNexus] Move character configuration into data assets
- [UBBBNexus] Remove character-side reload operation shadow state
## 2026-09-22
- [UBBBNexus] Unify character controllers as systems
- [UBBBNexus] Complete MonsterMass combat lifecycle and weapon damage input
- [UBBBNexus] docs: refine animation instance ownership
- [UBBBNexus] feat: add independent character appearance and customization flow
- [UBBBNexus] Add standalone equipment debug input actor
- [UBBBNexus] Rebuild equipment runtime around actor-owned inputs
- [UBBBNexus] Fix player input initialization and configure camera through blueprint
- [UBBBNexus] Align rifle blackboard with data constitution and document equipment boundaries
- [UBBBNexus] Move rifle tick and weapon montages to rifle runtime
- [UBBBNexus] refactor: simplify montage contributions
- [UBBBNexus] Fix rifle left hand IK animation fact production
## 2026-09-21
- [UBBBNexus] define domain data ownership laws
- [UBBBNexus] refactor: reorganize character source layout
- [UBBBNexus] refactor: isolate character network transport
- [UBBBNexus] docs: tighten animation instance law
- [UBBBNexus] refactor: move montage state into anim instance
- [UBBBNexus] docs: refine data blackboard law
- [UBBBNexus] docs: clarify domain data ownership laws
- [UBBBNexus] refactor runtime data into domain blackboards
- [UBBBNexus] reorganize runtime architecture laws
- [UBBBNexus] rename character runtime state to runtime data
- [UBBBNexus] simplify runtime data blackboards
## 2026-09-20
- [UBBBNexus] document character and equipment architecture laws
- [UBBBNexus] refactor character fact networking
- [UBBBNexus] refactor character and equipment execution architecture
- [UBBBNexus] refactor equipment static signatures
- [UBBBNexus] refactor character animation notifies
- [UBBBNexus] refactor character update orchestration
- [UBBBNexus] document character update pipeline
- [UBBBNexus] consolidate character update pipeline
- [UBBBNexus] refactor character tick ownership
- [UBBBNexus] remove character diagnostic output
- [UBBBNexus] refactor network system conventions
## 2026-09-19
- [UBBBNexus] Move parse input context into ParseSystem
- [UBBBNexus] Unify character input as per-frame discrete snapshot packets
- [UBBBNexus] Refactor character input into typed packets with banded priority arbitration
- [UBBBNexus] Add input packet template pair under Packets/_Template
- [UBBBNexus] Replace character input queue with fixed frame
- [UBBBNexus] Reorganize character input packets by semantics
## 2026-09-18
- [UBBBNexus] Move character input rules into packets
- [UBBBNexus] Simplify character input pipeline architecture
- [UBBBNexus] Move character core and runtime into explicit roots
- [UBBBNexus] Split character input packets by semantic domain
- [UBBBNexus] Simplify character input arbitration
- [UBBBNexus] Define character behavior rules beside typed inputs
## 2026-09-17
- [UBBBNexus] Unify equipment input and fixed system updates
- [UBBBNexus] Refactor equipment behavior into configurable fragments
- [UBBBNexus] 移除角色侧动画调试输出
- [UBBBNexus] Group instance internals and rename root actors
- [UBBBNexus] docs: complete cpp comments
- [UBBBNexus] Save current equipment animation workspace
- [UBBBNexus] Unify character inputs and blackboard pipeline with external player systems
## 2026-09-16
- [UBBBNexus] Remove redundant equipment action animation state
- [UBBBNexus] Route reload animation inputs through equipment arbitration
- [UBBBNexus] Centralize character shutdown and remove default equipment
- [UBBBNexus] Unify character equipment and autonomous equipment updates
- [UBBBNexus] Refactor equipment actions and publish animation facts by tick stage
- [UBBBNexus] Refactor character and equipment runtime architecture
- [UBBBNexus] Route equipment montages through character external API
- [UBBBNexus] Move animation instance bases to domain roots
## 2026-09-15
- [UBBBNexus] Bind weapon animation instance and keep aim facts on character
## 2026-09-14
- [UBBBNexus] Rename left hand IK validity to target
- [UBBBNexus] Add live left hand IK target query
- [UBBBNexus] Revert "Expose thread safe linked animation accessors"
- [UBBBNexus] Expose thread safe linked animation accessors
- [UBBBNexus] Fix delayed input callback capture
- [UBBBNexus] Refactor left hand IK to query live socket data
- [UBBBNexus] Route left hand IK through animation facts
- [UBBBNexus] Collapse linked animation instance into main base
## 2026-09-13
- [UBBBNexus] Expose equipment presentation for animation socket lookup
- [UBBBNexus] Remove left hand IK animation exposure
- [UBBBNexus] Cache equipment hand IK offset for animation
- [UBBBNexus] refactor: rename character late update tick
## 2026-09-12
- [UBBBNexus] Make left hand IK refresh unconditional
- [UBBBNexus] Update UBBBNexus runtime and controller
## 2026-09-11
- [UBBBNexus] refactor: unify equipment muzzle socket ownership
- [UBBBNexus] Unify weapon presentation on skeletal meshes
## 2026-09-06
- [UBBBNexus] refactor: move weapon action montages to domains
## 2026-09-05
- [UBBBNexus] Add weapon-driven left hand IK target
- [UBBBNexus] Fix equip reference bone initialization order
- [UBBBNexus] Keep equipment IK updates on existing interfaces
- [UBBBNexus] Add configurable left hand IK socket offset
- [UBBBNexus] Read left hand IK offset at runtime
- [UBBBNexus] Refresh weapon offsets during gameplay
- [UBBBNexus] Add debug switches for runtime offset refresh
- [UBBBNexus] Add runtime socket offsets for equipment IK
- [UBBBNexus] Treat IK offsets as socket local transforms
- [UBBBNexus] Refactor offsets to weapon socket space
- [UBBBNexus] Ignore empty quick access equipment slots
## 2026-09-03
- [UBBBNexus] 重构动画层选择职责
- [UBBBNexus] feat: 完成 UBBBNexus 动画与角色系统更新
## 2026-08-18
- [UBBBNexus] 限制非瞄准原地状态的身体转向
- [UBBBNexus] 限制原地转身信号仅在瞄准时生成
- [UBBBNexus] 新增CMC持续驱动动画参数
## 2026-08-17
- [UBBBNexus] 解除非瞄准移动的相机朝向锁定
- [UBBBNexus] 收紧角色网络与装备边界
- [UBBBNexus] 新增动画移动模式反推参数
- [UBBBNexus] 修正动画走跑档位判断
- [UBBBNexus] 改为记录上一次动画移动状态
- [UBBBNexus] 区分站立与蹲伏静止状态
- [UBBBNexus] 改为记录最后一次实际移动状态
- [UBBBNexus] 锁定急停前主动移动档位
- [UBBBNexus] 新增角色急停动画事实
- [UBBBNexus] 新增最后移动方向动画参数
## 2026-08-15
- [UBBBNexus] 恢复动画瞄准状态接口
- [UBBBNexus] 重建瞄准意图与IK权重链路
- [UBBBNexus] 修复换弹后瞄准姿势回退
- [UBBBNexus] Revert "Expose exact actor yaw delta"
- [UBBBNexus] Expose exact actor yaw delta
## 2026-08-14
- [UBBBNexus] Add hysteresis-based character facing system
- [UBBBNexus] Limit animation turn rate acceleration
- [UBBBNexus] Derive turn animation signals from actor rotation
- [UBBBNexus] Split character update around movement tick
- [UBBBNexus] Fix late update mesh variable naming
- [UBBBNexus] Fix turn rate frame timing
- [UBBBNexus] Stabilize animation turn rate
- [UBBBNexus] Gate late update by character lifecycle
- [UBBBNexus] Expose current foot height phase
- [UBBBNexus] Revert "Expose current foot height phase"
- [UBBBNexus] Expose direct turn direction signals
## 2026-08-11
- [UBBBNexus] Disable character IK during equipment actions
- [UBBBNexus] Ignore gameplay channels for dropped magazines
- [UBBBNexus] Smooth global aim presentation blocking
- [UBBBNexus] Route magazine notifies through equipment commands
- [UBBBNexus] Add physical magazine reload presentation
- [UBBBNexus] Restore equip left hand IK curve
- [UBBBNexus] Keep left hand IK active while firing
- [UBBBNexus] Cache equipment IK pose at equip time
- [UBBBNexus] Add MassEntity monster V0 and sync animation data
- [UBBBNexus] Keep aim IK active while firing
- [UBBBNexus] Keep reload IK disabled for remote presentation
## 2026-08-10
- [UBBBNexus] fix(animation): align turn rate frame timing
- [UBBBNexus] fix: align character updates with movement tick
- [UBBBNexus] refactor: make character movement phase explicit
- [UBBBNexus] refactor: run character pipeline before movement
- [UBBBNexus] feat: expose actual character turn rate
- [UBBBNexus] feat(character): add crouch movement flow
- [UBBBNexus] feat(character): split crouch movement profiles
- [UBBBNexus] Revert "feat(character): split crouch movement profiles"
- [UBBBNexus] fix(character): block jumping while crouched
- [UBBBNexus] fix: route aiming rotation through character movement
- [UBBBNexus] refactor(equipment): finalize runtime data move
## 2026-08-09
- [UBBBNexus] refactor: isolate character facing system
- [UBBBNexus] refactor: centralize equipment upper body animations
- [UBBBNexus] refactor: centralize character aiming state
- [UBBBNexus] docs: add standalone nexus readme
- [UBBBNexus] refactor: merge character facing into aim system
- [UBBBNexus] refactor: simplify aiming facing behavior
- [UBBBNexus] feat: add equipment aiming animation domin
- [UBBBNexus] refactor: expose complete left hand ik target
- [UBBBNexus] fix: restore aiming body turn control
- [UBBBNexus] refactor: clarify facing mode flow
## 2026-08-08
- [UBBBNexus] refactor: add aiming strafe locomotion profile
- [UBBBNexus] feat: expose firing fact to anim instance
- [UBBBNexus] feat: expose equipped upper body animation
- [UBBBNexus] refactor: centralize character facing control
- [UBBBNexus] feat: face aim direction while firing
- [UBBBNexus] docs: clarify facing turn hysteresis
- [UBBBNexus] refactor: separate equipped locomotion tuning
- [UBBBNexus] refactor: move aim direction into facing system
## 2026-08-07
- [UBBBNexus] refactor: make locomotion state priority explicit
- [UBBBNexus] refactor: expose animation locomotion facts
- [UBBBNexus] chore: update aim config and equipment instance
- [UBBBNexus] fix: stabilize gait and landing animation states
- [UBBBNexus] refactor: merge empty hand airborne states
- [UBBBNexus] refactor: split locomotion states by equipment stance
- [UBBBNexus] feat: add character jump support
- [UBBBNexus] refactor: clarify locomotion animation state processing
## 2026-08-06
- [UBBBNexus] refactor: streamline anim instance blueprint surface
- [UBBBNexus] refactor: rebuild locomotion presentation states in animation system
## 2026-08-05
- [UBBBNexus] fix: preserve continuous animation state
- [UBBBNexus] chore: clarify missing equipment catalog error
- [UBBBNexus] refactor: make equipment instance own presentation
- [UBBBNexus] refactor: move equipment behavior into system
## 2026-08-04
- [UBBBNexus] 剥离Definition方法为纯数据容器 装备与子弹挪入Item/Template
- [UBBBNexus] Fragments按领域拆入Template装备与武器目录
- [UBBBNexus] refactor: flatten equipment fragments
- [UBBBNexus] 新增显式可装备操作域
- [UBBBNexus] refactor: finalize and group equipment fragments
- [UBBBNexus] fix: call equipment domins through struct references
- [UBBBNexus] refactor: restore polymorphic equipment domins
- [UBBBNexus] refactor: separate equipment domain architecture
- [UBBBNexus] fix: store equipment fragments as value structs
- [UBBBNexus] feat: drive left hand IK with equip curve
- [UBBBNexus] refactor: strengthen equipment instance validation
- [UBBBNexus] refactor: remove equipment domain base
- [UBBBNexus] 重构物品操作域与单一实例根
## 2026-08-03
- [UBBBNexus] 重构装备镜像与句柄还原
- [UBBBNexus] 整理物品模块目录结构
- [UBBBNexus] 调整物品目录层级 目录Catalog重命名为CatalogSystem
- [UBBBNexus] 整理现有代码排版
- [UBBBNexus] 保存现有物品代码整理
- [UBBBNexus] 收口装备实例初始化生命周期
- [UBBBNexus] 补充物品定义注释
- [UBBBNexus] 简化角色背包与快捷访问设计
- [UBBBNexus] docs(UBBBNexus): 为全部头文件函数声明补充Doxygen中文注释
- [UBBBNexus] 整理物品系统声明排版
- [UBBBNexus] 重构UBBBNexus数据声明边界
- [UBBBNexus] 统一数据定义目录层级
- [UBBBNexus] 武器Fragment改为显式命名插槽装配
- [UBBBNexus] 重构装备实例根与镜像边界
- [UBBBNexus] 规范物品领域数据根与叶子声明
## 2026-08-02
- [UBBBNexus] Rename BBBNexus to UBBBNexus and fix include paths
