# AA轮实验脚本恢复记录

第一次build在`_pad_m`（逻辑M尾行padding）名称上触发旧的`_pad数字`（LDS物理padding）解析假设，出现Python TypeError。不是GPU错误，没有AA轮性能数据。

修改build.py只在真正匹配`_pad数字`时添加H3_LDS_PAD；`_pad_m`仍独立添加H3_PAD_M。旧的pad8/pad16配置不变。已生成的有效构建不删除；恢复时仅补编译尚未成功的候选，再使用同一正确性门槛测量。
