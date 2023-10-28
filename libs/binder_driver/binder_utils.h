#pragma once
#include <string>
#include <cstdint>

static constexpr uint32_t TRANSACTION_base_value = static_cast< uint32_t >( 0x00000001 );
static constexpr uint32_t TRANSACTION_getService = TRANSACTION_base_value + 0;
static constexpr uint32_t TRANSACTION_checkService = TRANSACTION_base_value + 1;
static constexpr uint32_t TRANSACTION_addService = TRANSACTION_base_value + 2;
static constexpr uint32_t TRANSACTION_listServices = TRANSACTION_base_value + 3;
static constexpr uint32_t TRANSACTION_registerForNotifications = TRANSACTION_base_value + 4;
static constexpr uint32_t TRANSACTION_unregisterForNotifications = TRANSACTION_base_value + 5;
static constexpr uint32_t TRANSACTION_isDeclared = TRANSACTION_base_value + 6;
static constexpr uint32_t TRANSACTION_getDeclaredInstances = TRANSACTION_base_value + 7;
static constexpr uint32_t TRANSACTION_updatableViaApex = TRANSACTION_base_value + 8;
static constexpr uint32_t TRANSACTION_getConnectionInfo = TRANSACTION_base_value + 9;
static constexpr uint32_t TRANSACTION_registerClientCallback = TRANSACTION_base_value + 10;
static constexpr uint32_t TRANSACTION_tryUnregisterService = TRANSACTION_base_value + 11;
static constexpr uint32_t TRANSACTION_getServiceDebugInfo = TRANSACTION_base_value + 12;


