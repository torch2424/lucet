use crate::context::Context;
use libc::{
    c_void, ucontext_t, REG_R12, REG_R13, REG_R14, REG_R15, REG_RBP, REG_RBX, REG_RDI, REG_RIP,
    REG_RSP,
};
use std::arch::x86_64::{__m128, _mm_loadu_ps};

#[derive(Clone, Copy, Debug)]
pub struct UContextPtr(*const ucontext_t);

impl UContextPtr {
    #[inline]
    pub fn new(ptr: *const c_void) -> Self {
        assert!(!ptr.is_null(), "non-null context");
        UContextPtr(ptr as *const ucontext_t)
    }

    #[inline]
    pub fn get_ip(self) -> *const c_void {
        let mcontext = &unsafe { *(self.0) }.uc_mcontext;
        mcontext.gregs[REG_RIP as usize] as *const _
    }

    pub fn save_to_context(&self, ctx: &mut Context) {
        let mcontext = &unsafe { *(self.0) }.uc_mcontext;
        ctx.gpr.rbx = mcontext.gregs[REG_RBX as usize] as u64;
        ctx.gpr.rsp = mcontext.gregs[REG_RSP as usize] as u64;
        ctx.gpr.rbp = mcontext.gregs[REG_RBP as usize] as u64;
        ctx.gpr.rdi = mcontext.gregs[REG_RDI as usize] as u64;
        ctx.gpr.r12 = mcontext.gregs[REG_R12 as usize] as u64;
        ctx.gpr.r13 = mcontext.gregs[REG_R13 as usize] as u64;
        ctx.gpr.r14 = mcontext.gregs[REG_R14 as usize] as u64;
        ctx.gpr.r15 = mcontext.gregs[REG_R15 as usize] as u64;

        let fpregs = &unsafe { *(mcontext.fpregs) };
        let xmms = fpregs._xmm[0..8]
            .iter()
            .map(|reg| unsafe { _mm_loadu_ps(reg.element.as_ptr() as *const u32 as *const _) })
            .collect::<Vec<__m128>>();
        ctx.fpr.xmm0 = xmms[0];
        ctx.fpr.xmm1 = xmms[1];
        ctx.fpr.xmm2 = xmms[2];
        ctx.fpr.xmm3 = xmms[3];
        ctx.fpr.xmm4 = xmms[4];
        ctx.fpr.xmm5 = xmms[5];
        ctx.fpr.xmm6 = xmms[6];
        ctx.fpr.xmm7 = xmms[7];
    }
}

// TODO: refactor uses of these types so that a deref instance can make sense, then move the methods
// from the ptr type into the target-specific ucontext type
//
// impl std::ops::Deref for UContextPtr {
//     type Target = UContext;
//     fn deref(&self) -> &Self::Target {
//         &unsafe { UContext::new(self.0 as *const c_void) }
//     }
// }

#[repr(C)]
#[derive(Clone, Copy)]
pub struct UContext {
    context: ucontext_t,
}

impl UContext {
    #[inline]
    pub fn new(ptr: *const c_void) -> Self {
        UContext {
            context: *unsafe {
                (ptr as *const ucontext_t)
                    .as_ref()
                    .expect("non-null context")
            },
        }
    }

    pub fn as_ptr(&self) -> UContextPtr {
        UContextPtr::new(&self.context as *const _ as *const _)
    }
}

impl Into<UContext> for UContextPtr {
    #[inline]
    fn into(self) -> UContext {
        UContext {
            context: unsafe { *(self.0) },
        }
    }
}
