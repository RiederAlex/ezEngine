#include <RendererFoundation/RendererFoundationPCH.h>

#include <RendererFoundation/CommandEncoder/ComputeCommandEncoder.h>
#include <RendererFoundation/Device/Device.h>
#include <RendererFoundation/Resources/Buffer.h>

ezGALComputeCommandEncoder::ezGALComputeCommandEncoder(ezGALDevice& ref_device, ezGALCommandEncoderState& ref_state, ezGALCommandEncoderCommonPlatformInterface& ref_commonImpl, ezGALCommandEncoderComputePlatformInterface& ref_computeImpl)
  : ezGALCommandEncoder(ref_device, ref_state, ref_commonImpl)
  , m_ComputeImpl(ref_computeImpl)
{
}

ezGALComputeCommandEncoder::~ezGALComputeCommandEncoder() = default;

ezResult ezGALComputeCommandEncoder::Dispatch(ezUInt32 uiThreadGroupCountX, ezUInt32 uiThreadGroupCountY, ezUInt32 uiThreadGroupCountZ)
{
  AssertRenderingThread();

  EZ_ASSERT_DEBUG(uiThreadGroupCountX > 0 && uiThreadGroupCountY > 0 && uiThreadGroupCountZ > 0, "Thread group counts of zero are not meaningful. Did you mean 1?");

  CountDispatchCall();
  return m_ComputeImpl.DispatchPlatform(uiThreadGroupCountX, uiThreadGroupCountY, uiThreadGroupCountZ);
}

ezResult ezGALComputeCommandEncoder::DispatchIndirect(ezGALBufferHandle hIndirectArgumentBuffer, ezUInt32 uiArgumentOffsetInBytes)
{
  AssertRenderingThread();

  const ezGALBuffer* pBuffer = GetDevice().GetBuffer(hIndirectArgumentBuffer);
  EZ_ASSERT_DEV(pBuffer != nullptr, "Invalid buffer handle for indirect arguments!");

  CountDispatchCall();
  return m_ComputeImpl.DispatchIndirectPlatform(pBuffer, uiArgumentOffsetInBytes);
}

void ezGALComputeCommandEncoder::ClearStatisticsCounters()
{
  ezGALCommandEncoder::ClearStatisticsCounters();

  m_uiDispatchCalls = 0;
}
