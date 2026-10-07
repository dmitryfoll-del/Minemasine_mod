# Minemasine_mod — Change Log

## 2026-10-07

### Configuration initialization
- Changed CN_MachineConfigManager so machine JSON files are created during server startup instead of first machine initialization.
- Added CN_MachineConfigManager.Initialize().
- Added initial machine registration for CN_OreExtractor.
- The configuration directory is created during server startup: `$profile:ColdNight_SRV_Data\\Mine_Mashines\\`.
- Existing JSON files are preserved and are loaded normally.
- Added CN_MiningMachines_Server/Scripts/4_World/CN_MiningMachinesServer.c to initialize the configuration manager from MissionServer.OnInit().
- Kept EEInit() in the server-side machine class responsible only for loading the already-created machine configuration.
