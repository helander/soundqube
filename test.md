# 📦 Generic Block Diagrams with Mermaid

This document demonstrates how to create general-purpose block diagrams using [Mermaid](https://mermaid.js.org/)'s `flowchart` feature. GitHub renders these diagrams automatically.

---

## 📊 Basic Block Diagram

```mermaid
flowchart LR
  A[User Interface] --> B[Application Logic]
  B --> C[API Gateway]
  C --> D[Database]
  C --> E[Auth Service]
```

---

## 🧱 Block Diagram with Subsystems

```mermaid
flowchart LR
  subgraph Client Side
    A[Web UI] --> B[Frontend Controller]
  end

  subgraph Server Side
    C[API Layer] --> D[Business Logic]
    D --> E[Database]
    D --> F[Email Service]
  end

  B --> C
```

---

## 🎨 Styled Block Diagram

```mermaid
flowchart LR
  A[Compute Node]:::compute --> B((Storage)):::storage
  A --> B

  classDef compute fill=#d3f,stroke=#333,stroke-width=2px;
  classDef storage fill=#fdd,stroke=#900,stroke-width=2px;
```

---

## 🧩 Node Shape Variants

```mermaid
flowchart TD
  A[Rectangle] --> B((Circle))
  B --> C{Diamond}
  C --> D>Asymmetric]
  D --> E[[Subroutine]]
```

---

## 📦 External Systems with Grouping

```mermaid
flowchart LR
  A[App Core] --> B[Auth Service]
  A --> C[Database]

  subgraph External Systems
    D[Stripe Payment]
    E[SMTP Server]
  end

  B --> D
  B --> E
```

---

## 📚 Resources

- [Mermaid Docs](https://mermaid.js.org/syntax/flowchart.html)
- [GitHub Mermaid Support](https://github.blog/2022-02-14-include-diagrams-markdown-files-mermaid/)
