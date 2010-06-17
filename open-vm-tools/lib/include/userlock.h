/*********************************************************
 * Copyright (C) 2009 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 2.1 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the Lesser GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 *********************************************************/

#ifndef _USERLOCK_H_
#define _USERLOCK_H_


#define INCLUDE_ALLOW_USERLEVEL
#define INCLUDE_ALLOW_VMCORE
#include "includeCheck.h"

#include "vm_atomic.h"
#include "vm_basic_types.h"
#include "vm_basic_defs.h"
#include "vthreadBase.h"

typedef struct MXUserExclLock   MXUserExclLock;
typedef struct MXUserRecLock    MXUserRecLock;
typedef struct MXUserRWLock     MXUserRWLock;
typedef struct MXUserCondVar    MXUserCondVar;

/*
 * Exclusive ownership lock
 */

MXUserExclLock *
MXUser_CreateExclLock(const char *name,
                    MX_Rank rank);

void MXUser_AcquireExclLock(MXUserExclLock *lock);
Bool MXUser_TryAcquireExclLock(MXUserExclLock *lock);
void MXUser_ReleaseExclLock(MXUserExclLock *lock);
void MXUser_DestroyExclLock(MXUserExclLock *lock);
Bool MXUser_IsCurThreadHoldingExclLock(const MXUserExclLock *lock);

MXUserExclLock *
MXUser_CreateSingletonExclLock(Atomic_Ptr *lockStorage,
                               const char *name,
                               MX_Rank rank);

Bool MXUser_ControlExclLock(MXUserExclLock *lock,
                            uint32 command,
                            ...);

MXUserCondVar *
MXUser_CreateCondVarExclLock(MXUserExclLock *lock);

void MXUser_WaitCondVarExclLock(MXUserExclLock *lock,
                                MXUserCondVar *condVar);


/*
 * Recursive lock.
 */

MXUserRecLock *
MXUser_CreateRecLock(const char *name,
                     MX_Rank rank);

void MXUser_AcquireRecLock(MXUserRecLock *lock);
Bool MXUser_TryAcquireRecLock(MXUserRecLock *lock);
void MXUser_ReleaseRecLock(MXUserRecLock *lock);
void MXUser_DestroyRecLock(MXUserRecLock *lock);
Bool MXUser_IsCurThreadHoldingRecLock(const MXUserRecLock *lock);

MXUserRecLock *
MXUser_CreateSingletonRecLock(Atomic_Ptr *lockStorage,
                              const char *name,
                              MX_Rank rank);

Bool MXUser_ControlRecLock(MXUserRecLock *lock,
                           uint32 command,
                           ...);

MXUserCondVar *
MXUser_CreateCondVarRecLock(MXUserRecLock *lock);

void MXUser_WaitCondVarRecLock(MXUserRecLock *lock,
                               MXUserCondVar *condVar);

/*
 * Read-write lock
 */

MXUserRWLock *
MXUser_CreateRWLock(const char *name,
                    MX_Rank rank);

void MXUser_AcquireForRead(MXUserRWLock *lock);
void MXUser_AcquireForWrite(MXUserRWLock *lock);
void MXUser_ReleaseRWLock(MXUserRWLock *lock);
void MXUser_DestroyRWLock(MXUserRWLock *lock);

#define MXUSER_RW_FOR_READ   0
#define MXUSER_RW_FOR_WRITE  1
#define MXUSER_RW_LOCKED     2

Bool MXUser_IsCurThreadHoldingRWLock(MXUserRWLock *lock,
                                     uint32 queryType);

/*
 * Generic conditional variable functions.
 */

void MXUser_SignalCondVar(MXUserCondVar *condVar);
void MXUser_BroadcastCondVar(MXUserCondVar *condVar);
void MXUser_DestroyCondVar(MXUserCondVar *condVar);

/*
 * MXUser_Control[Excl, Rec] commands
 */

#define MXUSER_CONTROL_ACQUISITION_HISTO 0  // numBins, binWidth
#define MXUSER_CONTROL_HELD_HISTO        1  // numBins, binWidth

#if defined(VMX86_VMX)
#include "mutex.h"

MXUserRecLock *MXUser_BindMXMutexRec(MX_MutexRec *mutex);
MX_MutexRec   *MXUser_GetRecLockVmm(const MXUserRecLock *lock);
MX_Rank        MXUser_GetRecLockRank(const MXUserRecLock *lock);

MXUserRecLock *MXUser_InitFromMXRec(const char *name, MX_MutexRec *mutex,
                                    MX_Rank rank, Bool isBelowBull);

#if defined(__i386__) || defined(__x86_64__)
#if defined(VMX86_STATS)
#define MXUSER_STATS  // stats "only inside the VMX" when requested
#endif
#endif  // X86 and X86-64

#endif  // VMX86_VMX

#if defined(VMX86_DEBUG)
#define MXUSER_DEBUG  // debugging "everywhere" when requested
#endif

#if defined(MXUSER_DEBUG)
Bool MXUser_AnyLocksHeld(VThreadID tid);
void MXUser_TryAcquireFailureControl(Bool (*func)(const char *lockName));
#endif

#if defined(MXUSER_STATS)
void MXUser_StatisticsControl(double contentionRatio,
                              uint64 minCount);
void MXUser_LogStats(unsigned epoch);
#endif

extern void MXUser_SetInPanic(void);
extern Bool MXUser_InPanic(void);

#endif  // _USERLOCK_H_