<script lang="ts">
  import { onMount } from "svelte";
  import Button from "./Button.svelte";
  import { firstTimeUser } from "../stores/firstTimeUserStore";
  import {
    ImageUpIcon,
    WandSparklesIcon,
    SettingsIcon,
    ShieldCheckIcon,
  } from "@lucide/svelte";

  let dialog: HTMLDialogElement;
  let currentStep = $state(0);

  const steps = [
    {
      title: "Welcome to the Wiwitool! ",
      text: "The easiest way to bring your own pictures and music into Minecraft. Whether you want custom paintings for your base or custom tunes for your jukebox, you can build a mix-and-match data and resource pack in seconds.",
      icon: WandSparklesIcon,
    },
    {
      title: "Wiwionboarding one",
      text: "miau",
      icon: ImageUpIcon,
    },
    {
      title: "Wiwionboarding 2",
      text: "miau",
      icon: SettingsIcon,
    },
    {
      title: "Wiwionboarding 3",
      text: "miau",
      icon: ShieldCheckIcon,
    },
  ];

  onMount(() => {
    if ($firstTimeUser) dialog.showModal();
  });

  function nextStep() {
    if (currentStep < steps.length - 1) {
      currentStep++;
    } else {
      closeOverlay();
    }
  }

  function prevStep() {
    if (currentStep > 0) {
      currentStep--;
    }
  }

  function closeOverlay() {
    dialog.close();
    if (!import.meta.env.DEV) firstTimeUser.set(false);
  }
</script>

<dialog
  bind:this={dialog}
  class="onboarding-dialog"
  oncancel={(e) => {
    e.preventDefault();
    closeOverlay();
  }}
>
  <div class="modal-content onboarding-content">
    <div class="step-indicator">
      {#each steps as _, i}
        <div class="dot" class:active={i === currentStep}></div>
      {/each}
    </div>

    <div class="icon-container">
      <svelte:component
        this={steps[currentStep].icon}
        size="64"
        strokeWidth="1.5"
        class="step-icon"
      />
    </div>

    <h2 style="margin-top: 0; text-align: center;">
      {steps[currentStep].title}
    </h2>
    <p style="text-align: center; font-size: 110%; opacity: 0.9;">
      {steps[currentStep].text}
    </p>

    <div
      class="modal-actions"
      style="margin-top: 30px; justify-content: space-between;"
    >
      <div>
        {#if currentStep > 0}
          <Button transparent onclick={prevStep}>Back</Button>
        {:else}
          <Button transparent onclick={closeOverlay}>Skip</Button>
        {/if}
      </div>

      <Button intense={currentStep === steps.length - 1} onclick={nextStep}>
        {currentStep === steps.length - 1 ? "Get Started" : "Next"}
      </Button>
    </div>
  </div>
</dialog>

<style>
  .onboarding-dialog {
    padding: 0;
    border: none;
    border-radius: 12px;
    background: transparent;
  }

  .onboarding-dialog::backdrop {
    background: rgba(0, 0, 0, 0.75);
    backdrop-filter: blur(10px);
  }

  .onboarding-content {
    background: var(--bg-color, white);
    max-width: 500px;
    width: 90vw;
    padding: 40px;
    display: flex;
    flex-direction: column;
    align-items: center;
  }

  @media (prefers-color-scheme: dark) {
    .onboarding-content {
      background: #1a1a1a;
      color: #eee;
    }
  }

  .icon-container {
    margin: 20px 0;
    color: #646cff;
  }

  .step-indicator {
    display: flex;
    gap: 8px;
    margin-bottom: 20px;
  }

  .dot {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: #ccc;
    transition:
      background 0.3s,
      transform 0.3s;
  }

  .dot.active {
    background: #646cff;
    transform: scale(1.2);
  }

  @media (prefers-color-scheme: dark) {
    .dot {
      background: #555;
    }
    .dot.active {
      background: #646cff;
    }
  }
</style>
