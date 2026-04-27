---
name: user-manual-writer
description: Use when writing or updating end-user manuals, quick starts, troubleshooting guides, or release notes for the DinMeter motor controller.
---

# User Manual Writer Skill

## Required reading

- `AGENTS.md`
- `docs/standards/user_manual_policy.md`
- `docs/templates/end_user_manual_template.md`
- `docs/standards/safety_policy.md`
- relevant `docs/hardware/*/spec.md`

## Procedure

1. Identify user-visible changes.
2. Check implemented behavior in code.
3. Update supported operation only.
4. Mark unverified behavior as `未検証`.
5. Keep safety warnings explicit.
6. Do not expose school/internal/private information.
7. Do not document unimplemented features as available.

## Output

- Updated manual files
- List of assumptions
- List of unimplemented features that must not be documented as usable
