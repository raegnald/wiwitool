<script lang="ts">
  export let value: string;

  export let oninput: (e?: Event) => void = () => {};
  export let onchange: (e?: Event) => void = () => {};
</script>

<select id="painting-ratio" bind:value {oninput} {onchange}>
  <optgroup label="Square">
    <option value="ratio_1x1">1:1</option>
    <option value="ratio_2x2">2:2</option>
    <option value="ratio_3x3">3:3</option>
    <option value="ratio_4x4">4:4</option>
  </optgroup>

  <optgroup label="Wide">
    <option value="ratio_2x1" selected>2:1</option>

    <option value="ratio_3x1">3:1</option>
    <option value="ratio_3x2">3:2</option>

    <option value="ratio_4x1">4:1</option>
    <option value="ratio_4x2">4:2</option>
    <option value="ratio_4x3">4:3</option>
  </optgroup>

  <optgroup label="Tall">
    <option value="ratio_1x2">1:2</option>
    <option value="ratio_1x3">1:3</option>
    <option value="ratio_1x4">1:4</option>

    <option value="ratio_2x3">2:3</option>
    <option value="ratio_2x4">2:4</option>

    <option value="ratio_3x4">3:4</option>
  </optgroup>
</select>

<style></style>
